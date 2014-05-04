import vlc

playlist = []

playlist_pos = 0

Player = vlc.MediaPlayer()

vlc_instance = vlc.Instance()

vlc_mediaplayer = vlc_instance.media_player_new()

def play(track_loc):
        global playlist,playlist_pos

        if (track_loc is None) or (len(track_loc) == 0):
                vlc_mediaplayer.play()
        playlist.append(track_loc)
        playlist_pos = len(playlist) - 1
        _play_track(track_loc) 
        return 1;

def pause():

        vlc_mediaplayer.pause()
        return 1;

def skip(pos):
        global playlist_pos

        track_loc = ""
        new_pos = playlist_pos + 1*pos
        try : 
                track_loc = playlist[new_pos]
                playlist_pos = new_pos
        except IndexError:
                print "no track available"

        _play_track(track_loc)
        return 1;

def enqueue(track_loc):
        global playlist

        playlist.append(track_loc)
        return 1;

def _play_track(track_loc):
        if (track_loc is None) or (len(track_loc) == 0):
                return -1
        vlc_mediaplayer.stop()
        vlc_media = vlc_instance.media_new(track_loc)
        vlc_mediaplayer.set_media(vlc_media)
        vlc_mediaplayer.play()
        print track_loc 
        return 1
