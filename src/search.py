import os.path

from whoosh.index import create_in, open_dir
from whoosh.fields import *
from whoosh.qparser import QueryParser
from whoosh.query import Term 

from mutagen.mp4 import MP4
from mutagen.easymp4 import EasyMP4
#from mutagen.oggopus import OggOpus
#from mutagen.oggvorbis import OggVorbis
from mutagen.easyid3 import EasyID3

from config import DB_LOC

ALLOWED_EXTS = [".mp3",".m4a"]

def create_schema():

    print DB_LOC

    schema = Schema(title=TEXT(stored=True), path=ID(stored=True), 
            artist=TEXT(stored=True), album=TEXT(stored=True))

    if not os.path.exists(DB_LOC):
        os.mkdir(DB_LOC)

    ix = create_in(DB_LOC, schema)

def search(title,artist,album):
    if not os.path.exists(DB_LOC):
        create_schema()

    ix = open_dir(DB_LOC)

    with ix.searcher() as searcher:
        qry = QueryParser("title",ix.schema).parse(unicode(title))

        if artist:
            allow_q = Term("artist",unicode(artist))
        elif album:
            allow_q = Term("album",unicode(album))
        else:
            allow_q = None

        results = searcher.search(qry,filter=allow_q)

        for result in results:
            print result['path']

def index(directory):
    if not os.path.exists(DB_LOC):
        create_schema()

    ix = open_dir(DB_LOC)
    
    w = ix.writer()

    for filename in __find_files__(directory):
        _, file_ext = os.path.splitext(filename)
        title = artist = album = ""
      
        try:
            if file_ext == ".mp3":
                audio = EasyID3(filename)
            elif file_ext == ".m4a":
                audio = EasyMP4(filename)

            title = audio["title"]
            artist = audio["artist"]
            album = audio["album"]
        except Exception:
            print "wow!! there was an error parsing ", filename
            pass

        try :
            w.add_document(title=unicode(title),artist=unicode(artist),album=unicode(album),path=unicode(filename))
        except Exception:
            print "wow!! there was an err inserting ", filename 
            pass

    w.commit()

def __find_files__(directory):
    for root, dirs, files in os.walk(directory):
        for basename in files:
            if basename[0] == ".":
                continue
            _, file_ext = os.path.splitext(basename)
            if file_ext in ALLOWED_EXTS:
                filename = os.path.join(root,basename)
                yield filename
