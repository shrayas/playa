#ifndef LASTFM_HELPER_H
#define LASTFM_HELPER_H

#include <string>
#include <cstring>
#include <curl/curl.h>
#include <curl/easy.h>
#include <openssl/md5.h>
#include <json/json.hh>
#include <functional>
#include <chrono>
#include <thread>
#include "manager.h"

using std::string;
using manager::db_EXECUTE;
using std::map;

using namespace JSON;

namespace lastfm_helper
{
    bool lastfm_helper_init();
    bool lastfm_helper_shutdown();
    bool track_love(string,string);
    bool track_unlove(string,string);
    bool _has_lastfm();
    bool authenticate();

    string gen_qrystr(map<string,string>);
    string gen_apisig(map<string,string>,string);

    extern function <void(string)> on_user_should_auth;
}
#endif // LASTFM_HELPER_H
