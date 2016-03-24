#include "http.h"
#include <string.h>

Http::Http()
{
 multi_handle = curl_multi_init();
 handle_count = 0;
}

Http::~Http()
{
 curl_multi_cleanup(multi_handle);
}

void Http::Update()
{
 curl_multi_perform(multi_handle, &handle_count);
}

void Http::Status()
{
struct CURLMsg *m;
 
/* call curl_multi_perform or curl_multi_socket_action first, then loop
   through and check if there are any transfers that have completed */
fprintf(stderr,"S"); 
do {
  int msgq = 0;
  m = curl_multi_info_read(multi_handle, &msgq);
  if(m && (m->msg == CURLMSG_DONE)) {
    CURL *e = m->easy_handle;
fprintf(stderr,"transfer done\n");

HttpRequest *request=NULL;
curl_easy_getinfo(e, CURLINFO_PRIVATE, (HttpRequest **)&request);

    request->finish();



    curl_multi_remove_handle(multi_handle, e);
    curl_easy_cleanup(e);
  } else if (m) 
  {
fprintf(stderr,"some other message %d\n",m->msg);
  }
} while(m);
}

void Http::AddRequest(const char* uri)
{
  CURL* curl = NULL;
  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, uri);
 curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  curl_multi_add_handle(multi_handle, curl);
}
void Http::AddRequest(const char* uri, const char *payload)
{
  CURL* curl = NULL;
fprintf(stderr,"url [%s] payload [%s] size:%d\n",uri,payload,strlen(payload));
struct curl_slist *headers = NULL;
headers = curl_slist_append(headers, "Accept: application/json");
headers = curl_slist_append(headers, "Content-Type: application/json");
headers = curl_slist_append(headers, "charsets: utf-8");
  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, uri);
curl_easy_setopt(curl, CURLOPT_HEADER, true);
curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
 curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  curl_multi_add_handle(multi_handle, curl);
}
void Http::AddRequest(HttpRequest *request)
{
  curl_multi_add_handle(multi_handle, request->curl);
}
