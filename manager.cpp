#include "manager.h"

namespace manager{

  string db_MIGRATE[2] = 
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
    string allowed_extensions[] = {"mp3","m4a","wma","flac"};

    sqlite3_stmt *stmt_insert_tracks,*stmt_insert_tracks_data;
    int last_insert_row_id;

    if (tflag != FTW_F)
      return 0;

    // get the extension
    string file_ext = string(fpath).substr(string(fpath).find_last_of(".") + 1);

    // convert to lower case
    transform(file_ext.begin(), file_ext.end(), file_ext.begin(), ::tolower);

    if(find(begin(allowed_extensions), end(allowed_extensions), file_ext) 
        != end(allowed_extensions)){

      TagLib::FileRef f(fpath);

      if(!f.file()->isValid())
        return 0;

      sqlite3_prepare(
          db,
          "INSERT INTO tracks(title,album,artist,comment,genre,year,track,file_path) "\
          "values(?,?,?,?,?,?,?,?)",
          -1,
          &stmt_insert_tracks,
          0
          );

      sqlite3_bind_text(stmt_insert_tracks, 1,
          (f.tag()->title() == TagLib::String::null)  
          ? fpath : f.tag()->title().toCString(true), 1024, SQLITE_STATIC);

      sqlite3_bind_text(stmt_insert_tracks, 2,
          (f.tag()->album() == TagLib::String::null)
          ? "" : f.tag()->album().toCString(true), 1024, SQLITE_STATIC);

      sqlite3_bind_text(stmt_insert_tracks, 3,
          (f.tag()->artist() == TagLib::String::null)
          ? "" : f.tag()->artist().toCString(true), 1024, SQLITE_STATIC);

      sqlite3_bind_text(stmt_insert_tracks, 4,
          (f.tag()->comment() == TagLib::String::null)
          ? "" : f.tag()->comment().toCString(true), 1024, SQLITE_STATIC);

      sqlite3_bind_text(stmt_insert_tracks, 5,
          (f.tag()->comment() == TagLib::String::null)
          ? "" : f.tag()->comment().toCString(true), 1024, SQLITE_STATIC);

      sqlite3_bind_int(stmt_insert_tracks, 6, (f.tag()->year() == 0) ? 0 : f.tag()->year());

      sqlite3_bind_int(stmt_insert_tracks, 7, (f.tag()->track() == 0) ? 0 : f.tag()->track());

      sqlite3_bind_text(stmt_insert_tracks, 8, fpath  , 4096, SQLITE_STATIC);

      sqlite3_prepare(
          db,
          "INSERT INTO tracks_user_data(track_id) "\
          "values(?)",
          -1,
          &stmt_insert_tracks_data,
          0
          );

      if (sqlite3_step(stmt_insert_tracks) == SQLITE_DONE) {
        last_insert_row_id = sqlite3_last_insert_rowid(db);
        sqlite3_bind_int(stmt_insert_tracks_data, 1, last_insert_row_id);

        if (sqlite3_step(stmt_insert_tracks_data) != SQLITE_DONE)
          printf("Commit to tracks_user_data Failed! %s \n",sqlite3_errmsg(db));

        sqlite3_reset(stmt_insert_tracks_data);
      }
      else
        printf("Commit to tracks Failed! %s \n",sqlite3_errmsg(db));

      sqlite3_reset(stmt_insert_tracks);
    }
    return 0;           
  }

  /*static int db_callback(void *param, int argc, char **argv, char **azColName){
    for(int i=0; i<argc; i++)
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

    printf("here I go again");
    return 0;
    }*/

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
      return 1;
    }
    return 0;
  }

  void manager_shutdown(){
    sqlite3_close(db);
  }
}
