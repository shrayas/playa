import os.path

from whoosh.index import create_in
from whoosh.fields import *
from whoosh.qparser import QueryParser

from config import DB_LOC

ALLOWED_EXTS = [".mp3",".m4a",".ogg",".flac",".opus"]

def create_schema():

    schema = Schema(title=TEXT(stored=True), path=ID(stored=TRUE), 
            artist=TEXT(stored=TRUE), album=TEXT(stored=TRUE))

    if not os.path.exists(DB_LOC):
        os.mkdir(DB_LOC)

    ix = create_in(DB_LOC)

def search(title,artist,album):
    if not os.path.exists(DB_LOC):
        create_schema()

    ix = open_dir(DB_LOC)
    
    with ix.searcher() as searcher:
        query = QueryParser("title",ix.schema).parse(title)

        if artist:
            allow_q = query.Term("artist",artist)
        elif album:
            allow_q = query.Term("album",album)
        else:
            allow_q = None

        results = searcher.search(query,filter=allow_q)

        for result in results:
            print result

def index(directory):
    if not os.path.exists(DB_LOC):
        create_schema()

    ix = open_dir(DB_LOC)
    
    w = ix.writer()

#   TODO: ahem, get the meta data from the file and use it while inserting to the db
#    for filename in __find_files__(directory):
#        w.add_document()


def __find_files__(directory):
    for root, dirs, files in os.walk(directory):
        for basename in files:
            _, file_ext = os.path.splitext(basename)
            if file_ext in ALLOWED_EXTS:
                filename = os.path.join(root,basename)
                yield filename
