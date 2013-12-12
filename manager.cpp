#include "manager.h"

namespace manager{

  string db_MIGRATE[3] =
  {
    "CREATE TABLE IF NOT EXISTS tracks("  \
      "title TEXT NOT NULL," \
      "album TEXT," \
      "artist TEXT," \
      "comment TEXT," \
      "genre TEXT," \
      "year INT," \
      "track INT," \
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
    if (nftw(file_path.c_str(), manager::get_file_info, 20, FTW_DEPTH|FTW_PHYS) == -1) {
      printf("some error =======/n ");
    }
  }

  int get_file_info(const char *fpath, const struct stat *,int tflag, struct FTW *) {

    if (tflag != FTW_F)
      return 0;

    string allowed_extensions[] = {"mp3","m4a","wma","flac"};

    string tr_title, tr_artist, tr_album, tr_comment, tr_genre, 
           tr_title_sanitized, tr_album_sanitized, tr_artist_sanitized;

    int last_insert_row_id;

    // get the extension
    string file_ext = string(fpath).substr(string(fpath).find_last_of(".") + 1);

    // convert to lower case
    transform(file_ext.begin(), file_ext.end(), file_ext.begin(), ::tolower);

    if(find(begin(allowed_extensions), end(allowed_extensions), file_ext)
        != end(allowed_extensions)){

      TagLib::FileRef f(fpath);

      if(!f.file()->isValid())
        return 0;

/*      tr_title = str::replace(tr_title.start(),tr_title.end()) = ((f.tag()->title() == TagLib::String::null)
              ? fpath : f.tag()->title().to8Bit(true));*/

      if(db_EXECUTE("INSERT INTO " \
            "tracks(title,album,artist,comment,genre,year,track,file_path) VALUES(" \
            "'"+ ((f.tag()->title() == TagLib::String::null)
              ? fpath : f.tag()->title().to8Bit(true))+ "'," \
            "'"+((f.tag()->album() == TagLib::String::null)
              ? "" : f.tag()->album().to8Bit(true))+"'," \
            "'"+((f.tag()->artist() == TagLib::String::null)
              ? "" : f.tag()->artist().to8Bit(true))+"'," \
            "'"+((f.tag()->comment() == TagLib::String::null)
              ? "" : f.tag()->comment().to8Bit(true))+"'," \
            "'"+((f.tag()->genre() == TagLib::String::null)
              ? "" : f.tag()->genre().to8Bit(true))+"',"
            + std::to_string(f.tag()->year())+"," \
            + std::to_string(f.tag()->track())+"," \
            "'"+fpath+"'"
            ")") == 0)
        printf("commit to tracks failed, %s \n ", sqlite3_errmsg(db));
      if(db_EXECUTE("INSERT INTO " \
            "tracks_user_data(track_id) VALUES("+std::to_string(sqlite3_last_insert_rowid(db))+")")
          == 0)
        printf("commit to tracks_user_data failed, %s \n ", sqlite3_errmsg(db));
    }
    return 0;           
  }

  void get_display_files(int (*callback)(void*,int,char**,char**), void *data){
    db_EXECUTE("SELECT title,album,artist,rowid FROM tracks",callback,data);
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

  void manager_shutdown(){
    sqlite3_close(db);
  }
}
