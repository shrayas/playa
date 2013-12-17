#ifndef MANAGER_H
#define MANAGER_H

#include <ftw.h>
#include <string>
#include <algorithm>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>
#include <sqlite3.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

namespace manager{

  void manager_init();
  void manager_shutdown();

  void get_new_media_files(string);

  int db_EXECUTE(string);
  int db_EXECUTE(string,int (*)(void*,int,char**,char**), void (*));
  //static int db_callback(void *, int, char **, char **);

  int get_file_info(const char *, const struct stat *,int, struct FTW *);

  extern string db_MIGRATE[3];
  extern sqlite3 *db;
  extern string db_file_name;

}

#endif // MANAGER_H
