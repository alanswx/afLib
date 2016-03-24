#include "request.h"

class HueIPResponse: public HttpRequest {

 std::stringstream ip;
 public:
  HueIPResponse(const char *url);
  virtual void finish();
  virtual void get_ip(char *ip);
};
