#pragma once
#include <curl/curl.h>
#include <string>
#include <sstream>

class HttpRequest {

protected:
 int finished;
 std::stringstream uri;
 std::stringstream payload;

 char temp[4096];

 public:
  HttpRequest(const char *url);
  HttpRequest(const char *url, const char *payload);
  virtual void finish();
  virtual int completed();
  std::stringstream response;
  CURL* curl;

 private:
};
