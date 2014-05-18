import vlc

class TrackHandler:


    def play(self,track_loc):

        if (track_loc is None) or (len(track_loc) == 0):
            self.vlc_mediaplayer.play()
            self.playlist.append(track_loc)
            self.playlist_pos = len(self.playlist) - 1
            __play_track__(track_loc) 
            return 1;

    def pause(self):

        self.vlc_mediaplayer.pause()
        return 1;

    def skip(self,pos):

        track_loc = ""
        new_pos = slef.playlist_pos + 1*pos
        try : 
            track_loc = self.playlist[new_pos]
            self.playlist_pos = new_pos
        except IndexError:
            print "no track available"

        __play_track__(track_loc)
        return 1;

    def enqueue(self,track_loc):

        self.playlist.append(track_loc)
        return 1;

    def __play_track__(self,track_loc):
        if (track_loc is None) or (len(track_loc) == 0):
            return -1
        self.vlc_mediaplayer.stop()
        self.vlc_media = vlc_instance.media_new(track_loc)
        self.vlc_mediaplayer.set_media(vlc_media)
        self.vlc_mediaplayer.play()
        print track_loc 
        return 1

    def __media_endreached__(self,event):
        self.skip(1)

    def __init__(self):
        self.playlist = []
        self.playlist_pos = 0
        self.vlc_instance = vlc.Instance()
        self.Player = vlc.MediaPlayer()
        self.vlc_mediaplayer = vlc_instance.media_player_new()
        self.vlc_events = vlc_mediaplayer.event_manager()
        self.vlc_events.event_attach(vlc.EventType.MediaPlayerEndReached, 
                __media_endreached__)

