#include "request.h"
#include <sstream>
#include <iostream>
#include <string.h>


#define MAX_FILE_SIZE = 10485760 //10 MiB

size_t curl_to_string(void *ptr, size_t size, size_t count, void *requestptr)
{
    HttpRequest *request = (HttpRequest *)requestptr;

    std::string data((const char*) ptr, (size_t) size * count);
    request->response << data;
    return size * count;
} 

HttpRequest::HttpRequest(const char *url)
{
  finished=0;
  response.str( std::string() );
  response.clear();

  uri.str(url);


fprintf(stderr,"URI: [%s]\n",uri.str().c_str());
  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, uri.str().c_str());
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_to_string );
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
  curl_easy_setopt(curl, CURLOPT_PRIVATE, this);
}
HttpRequest::HttpRequest(const char* url, const char *thepayload)
{
  finished=0;
  uri.str(url);
  payload.str(thepayload);
  strcpy(temp,thepayload);

fprintf(stderr,"url [%s] payload [%s] size:%d\n",uri.str().c_str(),payload.str().c_str(),strlen(payload.str().c_str()));
response.str( std::string() );
response.clear();
struct curl_slist *headers = NULL;
headers = curl_slist_append(headers, "Accept: application/json");
headers = curl_slist_append(headers, "Content-Type: application/json");
headers = curl_slist_append(headers, "charsets: utf-8");
  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, uri.str().c_str());
curl_easy_setopt(curl, CURLOPT_HEADER, true);
curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, temp);
 curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_to_string );
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_PRIVATE, this);

}

void HttpRequest::finish()
{
    finished =1;
fprintf(stderr,"HttpRequest::finish called\n");
    
    std::cout << response.str()  <<std::endl;

}
int HttpRequest::completed()
{

   return finished;
}
