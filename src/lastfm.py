import requests
import hashlib

import config

#def scrobble():
# some code

#def now_playing():
# some code

#def love():
# some code

def __auth():
    auth_payload = {
            'username' : username,
            'password' : password,
            'api_key' : config.LASTFM_API_KEY,
            'method' : "auth.getMobileSession",
            }
    auth_payload['api_sig'] = __gen_api_sig(auth_payload)
    auth_payload['format'] = 'json'

    print auth_payload
    r = requests.post("https://ws.audioscrobbler.com/2.0/",data=auth_payload)
    print r.text

def __gen_api_sig(payload):
    unhashed_sig = "".join('%s%s' % (k,v) for k,v in sorted(payload.items()))

        unhashed_sig += config.LASTFM_API_SECRET

        return hashlib.md5(unhashed_sig.encode('utf-8')).hexdigest()

