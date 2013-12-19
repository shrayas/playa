#include "manager.h"

namespace manager{

  string db_MIGRATE[3] =
  {
    "CREATE TABLE IF NOT EXISTS tracks("  \
      "title TEXT NOT NULL," \
      "title_search TEXT NOT NULL," \
      "album TEXT," \
      "album_search TEXT," \
      "artist TEXT," \
      "artist_search TEXT," \
      "comment TEXT," \
      "genre TEXT," \
      "year INT," \
      "track INT," \
      "duration INT," \
      "file_path TEXT UNIQUE" \
      ");"
      ,
    "CREATE TABLE IF NOT EXISTS tracks_user_data(" \
      "track_id INT UNIQUE," \
      "playcount INT DEFAULT 0," \
      "rating INT DEFAULT 0" \
      ");"
      ,
    "CREATE TABLE IF NOT EXISTS config_vars(" \
      "key TEXT PRIMARY KEY," \
      "value TEXT" \
      ");"
  };

  sqlite3 *db = nullptr;
  string db_file_name = "/home/karthik/.config/playa/storage.db3";

  void manager_init() {

    mkdir("/home/karthik/.config",S_IRWXU | S_IRWXG);
    mkdir("/home/karthik/.config/playa",S_IRWXU | S_IRWXG);

    if(sqlite3_open(db_file_name.c_str(), &db))
      printf("error opening db file :: %s \n",sqlite3_errmsg(db));

    for(string sql: db_MIGRATE)
      db_EXECUTE(sql);

  }

  void get_new_media_files(string file_path){
    if (nftw(file_path.c_str(), manager::get_file_info, 20, FTW_DEPTH|FTW_PHYS) 
        == -1) {
      printf("some error =======/n ");
    }
  }

  // this is slow!!
  int get_file_info(const char *fpath, const struct stat *,
      int tflag, struct FTW *) {
    if (tflag != FTW_F)
      return 0;

    string allowed_extensions[] = {"mp3","m4a","wma","flac"};

    string tr_title, tr_artist, tr_album,
           tr_comment, tr_genre,
           tr_year, tr_track,
           tr_title_searchable, tr_album_searchable, tr_artist_searchable;

    // get the extension
    string file_ext = string(fpath).substr(string(fpath).find_last_of(".") + 1);

    // convert to lower case
    transform(file_ext.begin(), file_ext.end(), file_ext.begin(), ::tolower);

    if(find(begin(allowed_extensions), end(allowed_extensions), file_ext)
        != end(allowed_extensions)){

      TagLib::FileRef f(fpath);

      if(!f.file()->isValid())
        return 0;

      tr_title = sanitize_track_data(f.tag()->title(),fpath);
      tr_album = sanitize_track_data(f.tag()->album(),"");
      tr_artist = sanitize_track_data(f.tag()->artist(),"");
      tr_comment = sanitize_track_data(f.tag()->comment(),"");
      tr_genre = sanitize_track_data(f.tag()->genre(),"");
      tr_year = sanitize_track_data(std::to_string(f.tag()->year()),"");
      tr_track = sanitize_track_data(std::to_string(f.tag()->track()),"");

      tr_title_searchable = search_friendly(tr_title);
      tr_album_searchable = search_friendly(tr_album);
      tr_artist_searchable = search_friendly(tr_artist);

      if(db_EXECUTE("INSERT INTO " \
            "tracks(title,title_search,album,album_search,artist,artist_search,comment,genre,year,track,duration,file_path) VALUES(" \
            "'"+ tr_title + "'," \
            "'"+ tr_title_searchable + "'," \
            "'"+ tr_album + "'," \
            "'"+ tr_album_searchable + "'," \
            "'"+ tr_artist +"'," \
            "'"+ tr_artist_searchable +"'," \
            "'"+ tr_comment +"'," \
            "'"+ tr_genre +"',"
            + tr_year+"," \
            + tr_track+"," \
            + std::to_string(f.audioProperties()->length())+"," \
            "'"+fpath+"'"
            ")") == 0)
        printf("commit to tracks failed, %s \n ", sqlite3_errmsg(db));
      if(db_EXECUTE("INSERT INTO " \
            "tracks_user_data(track_id) VALUES(" \
            +std::to_string(sqlite3_last_insert_rowid(db))+")")
          == 0)
        printf("commit to tracks_user_data failed, %s \n ", sqlite3_errmsg(db));
    }
    return 0;           
  }

  int db_EXECUTE(string sql){
    return db_EXECUTE(sql,0,0);
  }

  int db_EXECUTE(string sql,int (*callback)(void*,int,char**,char**), void *data){
    char *_errMsg = 0;
    int rc = 0;
    sqlite3_exec(db, sql.c_str(), callback, data, &_errMsg);
    if( rc != SQLITE_OK ){
      printf(" ============ SQL error: %s =========== \n", _errMsg);
      sqlite3_free(_errMsg);
      return 0;
    }
    return 1;
  }

  string sanitize_track_data(TagLib::String taglib_data,string alt){

    string data = ((taglib_data == TagLib::String::null)
        ? alt : taglib_data.to8Bit(true));
    if(data.empty())
        return alt;

    string final_str;

    int i = -1;
    while(data[++i]){
        if(data[i] == '\\' || data[i] == '\'')
            final_str += '\\';
        final_str += data[i];
    }

    // todo real sanitisation
    return final_str;
  }

  string search_friendly(string data){
    // lowercase
    transform(data.begin(), data.end(), data.begin(), ::tolower);

    string final_str;
    int i=-1;
    // remove non alphabets
   while(data[++i])
    if(isalpha(data[i]))
        final_str += data[i];

    return final_str;
  }

  void manager_shutdown(){
    sqlite3_close(db);
  }
}
