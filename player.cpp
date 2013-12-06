#include "player.h"

static int c_set_media_cb(void *param, int argc, char **argv, char **azColName) {
  player::set_media(std::string(argv[0]));
  player::play();
  player::inc_playcount(std::string(argv[1]));
  return 1;
}

namespace player{

  function <void(float)> on_time_changed_cb = nullptr;
  int is_playing = -1;
  libvlc_instance_t *inst = nullptr;
  libvlc_media_player_t *mp = nullptr;
  libvlc_media_t *m = nullptr;
  libvlc_event_manager_t *event = nullptr;


  void player_init() {

    inst = libvlc_new (0, NULL);

    mp = libvlc_media_player_new(inst);

    is_playing = -1;

    event = libvlc_media_player_event_manager(mp);

    libvlc_event_attach(event, libvlc_MediaPlayerPlaying, player::callback, 0);
    libvlc_event_attach(event, libvlc_MediaPlayerPaused, player::callback, 0);
    libvlc_event_attach(event, libvlc_MediaPlayerStopped, player::callback, 0);
    libvlc_event_attach(event, libvlc_MediaPlayerEndReached, player::callback, 0);
    //libvlc_event_attach(event, libvlc_MediaPlayerTimeChanged, player::callback, 0);
    libvlc_event_attach(event, libvlc_MediaPlayerPositionChanged, player::callback, 0);
  }

  int play(void){
    libvlc_media_player_play(mp);

    is_playing = 1;

    return 1;
  }

  int pause(void){
    libvlc_media_player_pause(mp);

    is_playing = 2;

    return 1;
  }

  int set_media(string path){

    m = libvlc_media_new_path (inst, path.c_str());
    libvlc_media_player_set_media(mp,m);

    libvlc_media_release (m);

    return 1;
    //  return play();
  }

  int set_media(int row_id){
    string qr = "SELECT file_path,rowid FROM tracks WHERE rowid = "+std::to_string(row_id);
    db_EXECUTE(qr,c_set_media_cb,0);
    return 0;
  }

  void player_shutdown(void) {
    libvlc_media_player_stop (mp);

    libvlc_media_player_release (mp);

    libvlc_release (inst);
  }

  int inc_playcount(std::string row_id){
    // if update fails do an insert
    if (db_EXECUTE("UPDATE tracks_user_data SET playcount = playcount + 1 WHERE track_id = "+row_id) 
        == 1){
      db_EXECUTE("INSERT INTO tracks_user_data(track_id,playcount) VALUES("+row_id+",1); ");
    }
    return 1;
  }

  void callback(const libvlc_event_t *event, void *param)
  {
    switch(event->type){
      //    case libvlc_MediaPlayerEndReached:
      //    case libvlc_MediaPlayerStopped:
      //    case libvlc_MediaPlayerPaused:
      //      p->pause();
      //      break;
      case libvlc_MediaPlayerPositionChanged:
        {
          float post = libvlc_media_player_get_position(player::mp);
          on_time_changed_cb(post);
          break;
        }
      default:
        break;
    }
  }

}
