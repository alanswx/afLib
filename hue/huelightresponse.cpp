#include "huelightresponse.h"
#include <sstream>
#include <iostream>
#include <string.h>
#include <jansson.h>

#include "examples/hueSample/device-description.h"

HueLightResponse::HueLightResponse(const char *uri, iafLib *ptheLib)
: HttpRequest(uri)
{
  theLib=ptheLib;
}

HueLightResponse::HueLightResponse(const char *uri, const char *payload)
: HttpRequest(uri,payload)
{
}

#if 0
void HueLightResponse::setModuloLED(int on) {
    if (moduloLEDIsOn != on) {
        int16_t attrVal = on ? LED_ON : LED_OFF; // Modulo LED is active low
        if (theLib->setAttribute(AF_MODULO_LED, attrVal) != afSUCCESS) {
            printf("===Could not set Attribute\n");
        }
        moduloLEDIsOn = on;
    }
}
#endif

void HueLightResponse::finish()
{
 std::string lightName[3];
 int lightState[3];
 int lightBrightness[3];
    finished =1;
fprintf(stderr,"HueLightResponse::finish called\n");
    
    std::cerr << response.str()  <<std::endl;

fprintf(stderr,"FINISH - ABOUT TO PARSE [%s]\n",response.str().c_str());
 json_t *root;
    json_error_t error;
root = json_loads(response.str().c_str(), 0, &error);
if(!root)
{
    fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
    return ;
}
int i=1;
int done = 0;
do
{
  char name[255];
  sprintf(name,"%d",i);
  json_t *node = json_object_get(root, name);
    if(!json_is_object(node))
    {
      done = 1;
    }
    else
    {
      json_t *name= json_object_get(node, "name");
      if(json_is_string(name))
      {
         const char *name_s= json_string_value(name);
         fprintf(stderr,"light name: [%s]\n",name_s);
         lightName[i-1]=name_s;
      }
      json_t *type= json_object_get(node, "type");
      if(json_is_string(type))
      {
         const char *type_s= json_string_value(type);
         fprintf(stderr,"light type: [%s]\n",type_s);
      }
      json_t *modelid= json_object_get(node, "modelid");
      if(json_is_string(modelid))
      {
         const char *modelid_s= json_string_value(modelid);
         fprintf(stderr,"light modelid: [%s]\n",modelid_s);
      }
      json_t *manufacturername= json_object_get(node, "manufacturername");
      if(json_is_string(manufacturername))
      {
         const char *manufacturername_s= json_string_value(manufacturername);
         fprintf(stderr,"light manufacturername: [%s]\n",manufacturername_s);
      }
      json_t *state= json_object_get(node, "state");
      if(json_is_object(state))
      {
        json_t *on= json_object_get(state, "on");
        if(json_is_boolean(on))
        {
           if (json_is_true(on))
           {
              fprintf(stderr,"light is : [ON]\n");
              lightState[i-1]=1;
           }
           else
           {
              fprintf(stderr,"light is : [OFF]\n");
              lightState[i-1]=0;
           }
           
         }
        json_t *bri= json_object_get(state, "bri");
        if (json_is_integer(bri))
        {
             lightBrightness[i-1]=json_integer_value(bri);
        }
      }
    }

   i++;
} while (!done);


json_decref(root);

for (i=0;i<3;i++)
{
   int AF_Const = AF_LIGHT1LABEL;
   int AF_LConst = AF_LIGHT1;
int v = lightState[i];
bool attrVal = 0;
   switch(i)
   {
     case 0:
       AF_Const = AF_LIGHT1LABEL;
       AF_LConst = AF_LIGHT1;
 if (v) attrVal = 1; else attrVal =0;
if (theLib->setAttributeBool(AF_LIGHT1, attrVal) != afSUCCESS) {
            printf("===Could not set Attribute\n");
}
else
{
   printf("---Sent %d %d using %d\n",i,v,AF_LConst);
}
     break;
     case 1:
       AF_Const = AF_LIGHT2LABEL;
       AF_LConst = AF_LIGHT2;
 if (v) attrVal = 1; else attrVal =0;
if (theLib->setAttributeBool(AF_LConst, attrVal) != afSUCCESS) {
            printf("===Could not set Attribute\n");
}
else
{
   printf("---Sent %d %d using %d\n",i,v,AF_LConst);
}
     break;
     case 2:
       AF_Const = AF_LIGHT3LABEL;
       AF_LConst = AF_LIGHT3;
//v = lightBrightness[i];
 if (v) attrVal = 1; else attrVal =0;
if (theLib->setAttribute16(AF_LConst, lightBrightness[i]) != afSUCCESS) {
            printf("===Could not set Attribute\n");
}
if (theLib->setAttributeBool(AF_LConst, attrVal) != afSUCCESS) {
            printf("===Could not set Attribute\n");
}
else
{
   printf("---Sent %d %d using %d\n",i,v,AF_LConst);
}
     break;
   }
if (theLib->setAttribute(AF_Const, strlen(lightName[i].c_str()),lightName[i].c_str()) != afSUCCESS) {
            printf("===Could not set Attribute\n");
}
else
{
   printf("---Sent %d %s using %d\n",i,lightName[i].c_str(),AF_Const);
}


}
}
