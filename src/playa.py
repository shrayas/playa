"""
Playa, play an audio file.

Usage: 
    playa.py play FILE
    playa.py play_index FILE
    playa.py pause
    playa.py queue FILE
    playa.py next
    playa.py prev
    playa.py index DIR
    playa.py (-h | --help)
    playa.py quit

"""

import socket
import json
from docopt import docopt


PORT = 10165 # TODO make configurable
HOST = "localhost"
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def receive(data_json):
        import track_handler
        data = json.loads(data_json)
#        print data
        if data['play']:
                return track_handler.play(data['FILE'])
        elif data['play_index']:
                return track_handler.play_from_index(data['FILE'])
        elif data['pause']:
                return track_handler.pause()
        elif data['next']:
                return track_handler.skip(1)
        elif data['prev']:
                return track_handler.skip(-1)
        elif data['queue']:
                return track_handler.enqueue(data['FILE'])
        elif data['index']:
                return track_handler.index_dir(data['DIR'])
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
