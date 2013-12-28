#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <functional>
#include <future>
#include <vlc/vlc.h>
#include "manager.h"
#include "lastfm_helper.h"

using std::string;
using manager::db_EXECUTE;

namespace player{

  void player_init();
  int play(void);
  int pause(void);
  int set_media(string);
  int set_media(int);
  int inc_playcount(string);
  void seek_to(float);
  void player_shutdown();
  void callback(const libvlc_event_t*, void *);
  void setup_mp();
  int track_up(bool);
  string to_duration(int);

  // callback registrants
  extern function <void(float)> time_changed;
  extern function <void(int)> play_toggled;
  extern function <void(int)> end_reached;
  extern function <void(std::map<string,string>)> media_changed;
  extern function <void(int)> reset_gui;

  extern int is_playing;
  extern string current_track_loc;
  extern string current_track_rowid;
  extern libvlc_instance_t *inst;
  extern libvlc_media_player_t *mp;
  extern libvlc_event_manager_t *event;
  extern map<string,string> track_data;
  extern bool scrobbled;

}

#endif // PLAYER_H

