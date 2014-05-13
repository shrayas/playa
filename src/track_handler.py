# TODO make it a class

import os

import vlc

from fuzzywuzzy import process

def play_from_index(filename):
        global index

        file_key = process.extractOne(filename, index.keys())

        if file_key:
            print "playing: %s" % file_key[0]
            play(index[file_key[0]])
            return 1
        else:
            return 0

def index_dir(path):
        global index

        try:
            for root, dirs, files in os.walk(path):

                for f in files:
                    if f.lower().endswith(('.m4a','.mp3','.aac')):
                        index[f] = os.path.join(root, f)

            #print "indexed [%s]. No. of items: %s" % (path, len(index))
            return 1

        except:
            return 0

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

def _media_endreached(event):
        skip(1)

index = {}

playlist = []

playlist_pos = 0

Player = vlc.MediaPlayer()

vlc_instance = vlc.Instance()

vlc_mediaplayer = vlc_instance.media_player_new()

vlc_events = vlc_mediaplayer.event_manager()
vlc_events.event_attach(vlc.EventType.MediaPlayerEndReached, _media_endreached)

