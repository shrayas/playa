#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <functional>
#include <vlc/vlc.h>
#include "manager.h"

using std::string;
using manager::db_EXECUTE;

namespace player{

  void player_init();
  int play(void);
  int pause(void);
  int set_media(string);
  int set_media(int);
  int inc_playcount(string);
  void player_shutdown();
  static void callback(const libvlc_event_t*, void *param);

  extern function <void(float)> on_time_changed_cb;
  extern int is_playing;
  extern libvlc_instance_t *inst;
  extern libvlc_media_player_t *mp;
  extern libvlc_media_t *m;
  extern libvlc_event_manager_t *event;
}

#endif // PLAYER_H

