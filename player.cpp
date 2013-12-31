#include "player.h"

/**
  sqlite callback for setting the current track
 */
static int c_set_media_cb(void *, int , char **argv, char **) {
  player::current_track_loc = string(argv[0]);
  player::current_track_rowid = string(argv[1]);
  player::set_media(player::current_track_loc);
  return 1;
}

/**
  sqlite callback on track change, set the track_data
 */
static int c_media_changed_cb(void *, int argc, char **argv, char**azColName){
  for(int i=0; i<argc; i++)
    player::track_data[azColName[i]] = argv[i] ? argv[i] : NULL;
  player::media_changed(player::track_data);
  return 1;
}

namespace player{

  function <void(float)> time_changed = nullptr;
  function <void(int)> play_toggled = nullptr;
  function <void(int)> end_reached = nullptr;
  function <void(std::map<string,string>)> media_changed = nullptr;
  function <void(int)> reset_gui = nullptr;

  int is_playing = -1;

  libvlc_instance_t *inst = nullptr;
  libvlc_media_player_t *mp = nullptr;
  libvlc_event_manager_t *event = nullptr;
  string current_track_loc;
  string current_track_rowid;
  map<string,string> track_data;
  bool scrobbled = false;
  char const* vlc_argv[] = {
    "--ignore-config" // libvlc loads the system specific config file which is used by vlc too, avoid it, we dont have any use of a config (yet)
  };
  int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);
  // global @TODO: return a proper true/false/int

  /**
    setup
   */
  void player_init() {
    inst = libvlc_new (vlc_argc, vlc_argv);
    mp = libvlc_media_player_new(inst);
    // create one the first time just to release variables when the 1st track is played
    is_playing = -1;
  }

  /**
    play and increase the play_count in the db
   */
  int play(void){
    libvlc_media_player_play(mp);
    inc_playcount(current_track_rowid);
    is_playing = 1;

    return 1;
  }

  /**
    pause
   */
  int pause(void){
    libvlc_media_player_pause(mp);

    is_playing = 2;

    return 1;
  }

  /**
    setup the libvlc mp events
   */
  void setup_mp(){
    event = libvlc_media_player_event_manager(mp);

    libvlc_event_attach(event, libvlc_MediaPlayerPlaying, player::callback, 0);
    libvlc_event_attach(event, libvlc_MediaPlayerPaused, player::callback, 0);
    libvlc_event_attach(event, libvlc_MediaPlayerEndReached, player::callback, 0);
    libvlc_event_attach(event, libvlc_MediaPlayerTimeChanged, player::callback, 0);
    libvlc_event_attach(event, libvlc_MediaPlayerPositionChanged, player::callback, 0);
    libvlc_event_attach(event, libvlc_MediaPlayerEncounteredError, player::callback, 0);
  }

  /**
    set a new track to play
   */
  int set_media(string path){
    // forced to clear the frigging instance because when the state libvlc_Ended is reached
    // vlc_cond_wait goes into an infinite loop .. follow WaitUnused func...
    // @TODO investigate and fix
    if(libvlc_media_player_get_state(mp) != libvlc_Ended)
      libvlc_media_player_release(mp);

    libvlc_release (inst);
    inst = libvlc_new (vlc_argc, vlc_argv);

    mp = libvlc_media_player_new(inst);
    setup_mp();

    libvlc_media_player_set_media(mp,libvlc_media_new_path (inst, path.c_str()));

    // we'll play first since vlc does it asycn
    play();

    scrobbled = false;
    reset_gui(0);

    db_EXECUTE("SELECT t.title AS title, t.album AS album, t.artist AS artist,t.duration , tud.rating AS rating FROM tracks AS t INNER JOIN tracks_user_data AS tud ON tud.rowid = t.rowid WHERE t.file_path = '"+path+"'",c_media_changed_cb,0);

    return 1;
  }

  /**
    set the new track to play
    given a rowid
   */
  int set_media(int row_id){
    string qr = "SELECT file_path,rowid FROM tracks WHERE rowid = "+std::to_string(row_id);
    db_EXECUTE(qr,c_set_media_cb,0);
    return 0;
  }

  /**
    seek to a track position b/w 0 to 1
   */
  void seek_to(float position){
    libvlc_media_player_set_position(mp,position);
  }

  /**
    bye bye
   */
  void player_shutdown(void) {
    libvlc_media_player_stop (mp);

    libvlc_media_player_release (mp);

    libvlc_release (inst);
  }

  /**
    run query to increase the playcount
   */
  int inc_playcount(std::string row_id){
    // if update fails do an insert
    if (db_EXECUTE("UPDATE tracks_user_data SET playcount = playcount + 1 WHERE track_id = "+row_id)
        == 0){
      db_EXECUTE("INSERT INTO tracks_user_data(track_id,playcount) VALUES("+row_id+",1); ");
    }
    return 1;
  }

  /**
    thumbs a track
   */
  int track_up(bool up_down){

    // save thumbs_up in db
    if(up_down)
      db_EXECUTE("UPDATE tracks_user_data SET rating = 5 WHERE track_id = "+current_track_rowid);
    else
      db_EXECUTE("UPDATE tracks_user_data SET rating = 0 WHERE track_id = "+current_track_rowid);

    // send lastfm love req
    std::async(std::launch::async,lastfm_helper::track_love,up_down,track_data["title"],track_data["artist"],track_data["album"]);

    return 1;
  }

  /**
    on libvlc events
   */
  void callback(const libvlc_event_t *event, void *)
  {
    switch(event->type){
      case libvlc_MediaPlayerEndReached:
      {
        end_reached(0);
        scrobbled = false;
        break;
      }
      case libvlc_MediaPlayerPaused:
      {
        play_toggled(0);
        break;
      }
      case libvlc_MediaPlayerPlaying:
      {
        play_toggled(1);
        break;
      }
      case libvlc_MediaPlayerPositionChanged:
      {
        float post = libvlc_media_player_get_position(player::mp);
        // scrobble track when crossed half the length
        // yeah there are known issues...
        if(post > 0.5 && scrobbled == false){
          std::async(std::launch::async,lastfm_helper::scrobble,track_data["title"],track_data["artist"],track_data["album"]);
          scrobbled = true;
        }
        time_changed(post);
        break;
      }
      default:
        printf("there seems to be an error \n");
        break;
    }
  }

  /**
    convert seconds to duration string
    ex: "160 to 2:40"
   */
  string to_duration(int second){

    int hour,minute;
    string duration;

    hour=minute=0;

    if(second > 60){
      minute = second/60;
      second%= 60;
      if(minute > 60){
        hour = minute/60;
        minute%= 60;
      }
    }

    // TODO : show leading 0
    duration = to_string(minute) + ":" + to_string(second);
    if(hour > 0)
      duration = to_string(hour) + ":" + duration;

    return duration;
  }
}
