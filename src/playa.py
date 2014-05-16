"""Usage: playa.py (play | pause | quit | queue | next | prev) [FILE]

Playa, play an audio file.

Arguments:
  play [FILE] play audio
  queue [FILE] queue track 
  pause pause
  next goto next track
  prev goto prev track
  quit quit

Options:
  -h --help

"""

import socket
import json
from docopt import docopt
import track_handler

PORT = 10165 # TODO make configurable
HOST = "localhost"
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def receive(data_json):
        data = json.loads(data_json)
        track_handler = TrackHandler()
#        print data
        if data['play']:
                return track_handler.play(data['FILE'])
        elif data['pause']:
                return track_handler.pause()
        elif data['next']:
                return track_handler.skip(1)
        elif data['prev']:
                return track_handler.skip(-1)
        elif data['queue']:
                return track_handler.enqueue(data['FILE'])
        elif data['quit']:
                return 0 

if __name__ == '__main__':
        params = json.dumps(docopt(__doc__))
        try:
                s.bind((HOST,PORT))
                receive(params)
                while 1:
                        data,addr = s.recvfrom(1024)
                        if not receive(data):
                               break
                s.close()
                print "( ^_^)/"
        except socket.error as e:
                # Socket already bound to so just connect and send
                if e.errno == 48:
                    s.connect((HOST, PORT))
                    s.send(params)
                    s.close()
                else:
                    print e
