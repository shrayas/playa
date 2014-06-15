"""Usage: playa.py (play | pause | quit | queue | next | prev | index | search) [FILE] [ALBUM] [ARTIST]

Playa, play an audio file.

Arguments:
  play [FILE] play audio
  queue [FILE] queue track 
  pause pause
  index index current directory (recursive) 
  search search indexed files
  next goto next track
  prev goto prev track
  quit quit

Options:
  -h --help

"""

import socket
import json
import os
import sys
from docopt import docopt

from track_handler import TrackHandler
import search

PORT = 10165 # TODO make configurable
HOST = "localhost"
PIDFILE = "playa.pid"

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
cur_dir = os.path.dirname(os.path.realpath(__file__))

pid_absloc = os.path.join(cur_dir,PIDFILE)

def receive(data_json):
        data = json.loads(data_json)
#        print data
        if data['play']:
                return th.play(data['FILE'])
        elif data['pause']:
                return th.pause()
        elif data['next']:
                return th.skip(1)
        elif data['prev']:
                return th.skip(-1)
        elif data['queue']:
                return th.enqueue(data['FILE'])
        elif data['index']:
            search.index(data['FILE'])
            return 0
        elif data['quit']:
                return 0 

if __name__ == '__main__':
    data = docopt(__doc__) 
    if data['index']:
        search.index(data['FILE'])
    elif data['search']:
        search.search(data['FILE'],data['ARTIST'],data['ALBUM'])
    else:
        th = TrackHandler()
        params = json.dumps(docopt(__doc__))
       
        # we'll do it the good old way of checking a pid file, 
        # sockets are waayyy to low level
        if not os.path.isfile(pid_absloc):
                
                s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                s.bind((HOST,PORT))
                
                receive(params)
                
                pid = str(os.getpid())

                file(pid_absloc, 'w').write(pid)

                while 1:
                        data,addr = s.recvfrom(1024)
                        if not receive(data):
                               break

                s.shutdown(socket.SHUT_RDWR)
                s.close()

                os.unlink(PIDFILE)
                print "( ^_^)/"
        else:
                s.connect((HOST, PORT))
                s.send(params)
                s.shutdown(socket.SHUT_RDWR)
                s.close()
