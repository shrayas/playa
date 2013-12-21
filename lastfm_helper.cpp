#include "lastfm_helper.h"

// get the response from the curl request, returns a string
size_t static curl_response_callback(void *buffer, size_t size, size_t nmemb, void *userdata)
{
  *(string**)userdata = (string *)buffer;
  return size * nmemb;
}
size_t static curl_no_output(void *, size_t , size_t , void *){
  return 0;
}

namespace lastfm_helper{
  function <void(string)> on_user_should_auth = nullptr;
  string username = "";
  string api_key = "";
  string api_secret = "";
  string sk = "";
  string token = "";

  static int _setup_vars(void *, int , char **argv, char**){
    if(string(argv[0]) == "lastfm_api_key")
      lastfm_helper::api_key = argv[1] ? string(argv[1]) : NULL;
    if(string(argv[0]) == "lastfm_api_secret")
      lastfm_helper::api_secret = argv[1]? string(argv[1]) : NULL;
    if(string(argv[0]) == "lastfm_api_sk")
      lastfm_helper::sk = argv[1] ? string(argv[1]) : NULL;
    if(string(argv[0]) == "lastfm_api_username")
      lastfm_helper::username = argv[1] ? string(argv[1]) : NULL;

    return 0;
  }

  bool lastfm_helper_init(){
    db_EXECUTE("SELECT key,value FROM config_vars WHERE key LIKE 'lastfm%'",lastfm_helper::_setup_vars,0);
    return true;
  }

  bool lastfm_helper_shutdown() {
    return true;
  }

  bool track_love(string title,string artist){ return false;}

  bool track_unlove(string title,string artist){ return false;}

  bool scrobble(string title,string artist, string album){
    if(!_has_lastfm())
      return false;

    map<string,string> params;
    CURL *scrobble_req;
    string post_fields,tmp_api_sig;
    bool ret = true;
    params["api_key"] = api_key;
    params["sk"] = sk;
    params["track[0]"] = title;
    params["artist[0]"] = artist;
    if(!album.empty())
      params["album[0]"] = album;
    params["method"] = "track.scrobble";

    // create a fake stream to dump the time
    // and get it as a string
    std::stringstream ss_timestamp;
    ss_timestamp << time(NULL); // cross platform ??

    params["timestamp[0]"] = ss_timestamp.str();

    tmp_api_sig = gen_apisig(params,api_secret);
    params["api_sig"] = tmp_api_sig;

    post_fields = gen_qrystr(params);

    curl_global_init(CURL_GLOBAL_ALL);

    scrobble_req = curl_easy_init();

    if(scrobble_req){
      curl_easy_setopt(scrobble_req, CURLOPT_URL,"http://ws.audioscrobbler.com/2.0/");
      curl_easy_setopt(scrobble_req, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(scrobble_req, CURLOPT_POSTFIELDS,post_fields.c_str());
      curl_easy_setopt(scrobble_req, CURLOPT_WRITEFUNCTION, curl_no_output);

      if(curl_easy_perform(scrobble_req) != CURLE_OK){
        //              fprintf(stderr, "scrobble  failed: %s\n", curl_easy_strerror(res));
        ret = false;
      }
      curl_easy_cleanup(scrobble_req);
    }
    curl_global_cleanup();

    return ret;
  }

  bool _has_lastfm(){
    if(username == "" || sk == "")
      return false;
    return true;
  }

  bool authenticate(){
    // well they are required
    if(api_key == "" || api_secret == "")
      return false;

    CURL *rl_get_request_token;
    string token_response, sk_response, tmp_api_sig;
    int retry_count = 0;
    long http_code;
    map<string, string> getToken_urlparams,getSession_urlparams;
    Object sk_session_json;

    getToken_urlparams["api_key"] = api_key;
    getToken_urlparams["format"] = "json";
    getToken_urlparams["method"] = "auth.gettoken";

    // CURL, get a token for authing against
    string getToken_url = "https://ws.audioscrobbler.com/2.0/?"+gen_qrystr(getToken_urlparams);
    char * getToken_url_c = new char[getToken_url.length() + 1];
    std::strcpy(getToken_url_c,getToken_url.c_str());

    rl_get_request_token = curl_easy_init();

    if(rl_get_request_token) {
      curl_easy_setopt(rl_get_request_token, CURLOPT_URL,getToken_url_c);
      curl_easy_setopt(rl_get_request_token, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(rl_get_request_token, CURLOPT_WRITEFUNCTION, curl_response_callback);
      curl_easy_setopt(rl_get_request_token, CURLOPT_WRITEDATA, &token_response);

      if(curl_easy_perform(rl_get_request_token) != CURLE_OK){
        //        printf("not ok \n %s");
        return false;
      }
      curl_easy_cleanup(rl_get_request_token);
    }
    else
      return false;

    token = (string)(((Object)parse_string(token_response))["token"]);

    // the user will have to login so as to auth the token
    on_user_should_auth("https://www.last.fm/api/auth/?api_key="+api_key+"&token="+token);

    getSession_urlparams["api_key"] = api_key;
    getSession_urlparams["method"] = "auth.getSession";
    getSession_urlparams["token"] = token;
    tmp_api_sig = gen_apisig(getSession_urlparams,api_secret);
    getSession_urlparams["api_sig"] = tmp_api_sig;
    getSession_urlparams["format"] = "json";

    string getSession_url = "https://ws.audioscrobbler.com/2.0/?"+gen_qrystr(getSession_urlparams);
    char * getSession_url_c = new char[getSession_url.length() + 1];
    std::strcpy(getSession_url_c,getSession_url.c_str());

    // try 5 times (sleeping while the user is authing) to check if the user has signed and if the token goes through
    // get an sessionkey (sk) to use in future sessions
    do{
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      rl_get_request_token = curl_easy_init();
      if(rl_get_request_token) {
        curl_easy_setopt(rl_get_request_token, CURLOPT_URL,getSession_url_c);
        curl_easy_setopt(rl_get_request_token, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(rl_get_request_token, CURLOPT_WRITEFUNCTION, curl_response_callback);
        curl_easy_setopt(rl_get_request_token, CURLOPT_WRITEDATA, &sk_response);

        curl_easy_perform(rl_get_request_token);
        curl_easy_getinfo(rl_get_request_token, CURLINFO_HTTP_CODE, &http_code);
        curl_easy_cleanup(rl_get_request_token);

        printf(" session data ==> %s \n",sk_response.c_str());

        sk_session_json = ((Object)parse_string(sk_response))["session"];
        string username = (string)sk_session_json["name"];
        string sk = (string)sk_session_json["key"];

        if(username != "" && sk != "" ){
          lastfm_helper::sk = sk;
          lastfm_helper::username = username;
          break;
        }
        retry_count++;
      }
    } while (retry_count < 5);

    if(sk != ""){
      manager::db_EXECUTE("INSERT OR REPLACE INTO config_vars(key,value) VALUES" \
          "('lastfm_api_sk','"+ \
          sk+"')" \
          ",('lastfm_api_username','"+ \
          username+"')" \
          ";");
      return true;
    }
    return false;
  }

  string gen_qrystr(map<string,string> url_params){
    string qrystr = "";
    for (auto& param : url_params)
      qrystr += param.first + "=" + param.second + "&"; // we'll strip off the last &
    return qrystr.substr(0,qrystr.length()-1);
  }

  string gen_apisig(map<string,string> url_params, string api_secret){
    unsigned char digest[MD5_DIGEST_LENGTH];
    string api_sig = "";
    char md5_c[33];
    for (auto& param : url_params)
      api_sig += param.first + "" + param.second;
    api_sig += api_secret;
    MD5((const unsigned char *)api_sig.c_str(),api_sig.length(),digest);

    // convert int to hexadecimal
    for(int i = 0; i < 16; i++)
      sprintf(&md5_c[i*2], "%02x", (unsigned int)digest[i]);
    return string(md5_c);
  }
}
