import requests
import hashlib
import time

import config

#def scrobble():
#    some code

#def now_playing():
# some code

#def love():
# some code

class LastFM:
    
    sk = ''
    lastfm_endpoint = "https://ws.audioscrobbler.com/2.0/"

    def scrobble(self,track,artist):
        auth_payload = {
                'track[0]' : track,
                'artist[0]' : artist,
                'timestamp[0]' : str(int(time.time())),
                'api_key' : config.LASTFM_API_KEY,
                'sk' : self.sk,
                'method' : "track.scrobble",
                }

        auth_payload['api_sig'] = self.__gen_api_sig__(auth_payload)
        auth_payload['format'] = 'json'

        print auth_payload

        r = requests.post(self.lastfm_endpoint,data=auth_payload)
        print r.json()

    def __init__(self):
        auth_payload = {
                'username' : config.LASTFM_USERNAME,
                'password' : config.LASTFM_PASSWORD,
                'api_key' : config.LASTFM_API_KEY,
                'method' : "auth.getMobileSession",
                }
        auth_payload['api_sig'] = self.__gen_api_sig__(auth_payload)
        auth_payload['format'] = 'json'

        r = requests.post(self.lastfm_endpoint,data=auth_payload)
        request_response = r.json()
        self.sk = request_response['session']['key']

    def __gen_api_sig__(self,payload):
        # read http://www.last.fm/api/mobileauth#4
        unhashed_sig = "".join('%s%s' % (k,v) for k,v in sorted(payload.items()))

        unhashed_sig += config.LASTFM_API_SECRET

        return hashlib.md5(unhashed_sig.encode('utf-8')).hexdigest()

