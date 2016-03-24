/* start from http://www.godpatterns.com/2011/09/asynchronous-non-blocking-curl-multi.html */
#include <curl/curl.h>
#include "request.h"

class Http
{
 CURLM* multi_handle; 
 int handle_count;

 public:
  Http();
  ~Http();
  void Update();
  void Status();
  void AddRequest(const char* uri);
  void AddRequest(const char* uri, const char *payload);
  void AddRequest(HttpRequest *request);
};

