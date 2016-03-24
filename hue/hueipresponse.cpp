#include "hueipresponse.h"
#include <sstream>
#include <iostream>
#include <string.h>
#include <jansson.h>


HueIPResponse::HueIPResponse(const char *uri)
: HttpRequest(uri)
{
}


void HueIPResponse::get_ip(char *rip)
{

  strcpy(rip,ip.str().c_str());
 
}

void HueIPResponse::finish()
{
 std::string lightName[3];
 int lightState[3];
 int lightBrightness[3];
    finished =1;
fprintf(stderr,"HueIPResponse::finish called\n");
    
    std::cout << response.str()  <<std::endl;

 json_t *root;
    json_error_t error;
root = json_loads(response.str().c_str(), 0, &error);
if(!root)
{
    fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
    return ;
}

if (json_is_array(root))
{
fprintf(stderr,"json is array\n");
}

json_t *in = json_array_get(root,0);
void *iter = json_object_iter(in);
const char *key;
json_t*value;
while(iter)
{
 key = json_object_iter_key(iter);
 value= json_object_iter_value(iter);
 if (!strcasecmp(key,"internalipaddress"))
 {
 printf("key: %s value: %s\n",key,json_string_value(value));
         const char *name_s= json_string_value(value);
         ip.str(name_s);
 }
 iter = json_object_iter_next(in,iter);
}


json_decref(root);


}
