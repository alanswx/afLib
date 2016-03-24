#include "request.h"
#include "afLib.h"

class HueLightResponse: public HttpRequest {

 public:
  HueLightResponse(const char *url,iafLib *theLib);
  HueLightResponse(const char *url, const char *payload);
  virtual void finish();
 private:
  iafLib *theLib;
};
