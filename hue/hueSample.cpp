
/**
 * Copyright 2016 Alan Steremberg
 *
 */
#include "Arduino.h"
#include <string>
#include "iafLib.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include "huelightresponse.h"
#include "hueipresponse.h"
#include "http.h"
#include <unistd.h>


#include "gpiolib.h"
#include "examples/hueSample/device-description.h"

#include "linuxLog.h"
#include "linuxSPI.h"

iafLib *theLib = NULL;
char hue_ip[1024];
char hue_prefix[1024];
uint16_t moduleButtonValue = 1;  // Track the button value so we know when it has changed

Http http;



void lookuphue()
{
CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.meethue.com/api/nupnp");
    /* example.com is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
}

void isr_callback()
{
  fprintf(stdout,"isr_callback\n");
}

void onAttributeSet_callback(const uint8_t requestId, const uint16_t attributeId, const uint16_t valueLen, const uint8_t *value)
{
  HttpRequest *request = NULL;
  char hue_request[1024];
  fprintf(stdout,"onAttributeSet_callback: %d len: %d %x %s\n",attributeId,valueLen,*value,value);
  switch (attributeId)
  {
fprintf(stderr,"BUTTON PRESSED!\n");
break;
        case AF_LIGHT1:
          fprintf(stdout,"AF_LIGHT1 == %d\n",*value);
          sprintf(hue_request,"%slights/1/state",hue_prefix);
          if (*value)
  request = new HttpRequest(hue_request,"{ \"on\": true, \"hue\": 25500, \"sat\": 100, \"bri\": 100 }");
          else
  request = new HttpRequest(hue_request,"{ \"on\": false, \"hue\": 25500, \"sat\": 100, \"bri\": 100 }");

 http.AddRequest(request);

        break;
        case AF_LIGHT2:
          fprintf(stdout,"AF_LIGHT2 == %d\n",*value);
          sprintf(hue_request,"%slights/2/state",hue_prefix);
          if (*value)
  request = new HttpRequest(hue_request,"{ \"on\": true, \"hue\": 25500, \"sat\": 100, \"bri\": 100 }");
          else
  request = new HttpRequest(hue_request,"{ \"on\": false, \"hue\": 25500, \"sat\": 100, \"bri\": 100 }");

 http.AddRequest(request);

        break;
        case AF_LIGHT3:
          fprintf(stdout,"AF_LIGHT3 == %d\n",*value);
          sprintf(hue_request,"%slights/3/state",hue_prefix);
fprintf(stderr,"LIGHT 3 CHANGE STATE: %s\n",hue_request);
          if (*value)
  request = new HttpRequest(hue_request,"{ \"on\": true, \"hue\": 25500, \"sat\": 100, \"bri\": 100 }");
          else
  request = new HttpRequest(hue_request,"{ \"on\": false, \"hue\": 25500, \"sat\": 100, \"bri\": 100 }");

 http.AddRequest(request);

        break;
#if 0
        case AF_LIGHT1BRI:
        {
          uint16_t *light3Value= (uint16_t *) value;
          uint8_t *light3Value2= (uint8_t *) value;
          sprintf(hue_request,"%slights/1/state",hue_prefix);
          fprintf(stdout,"AF_LIGHT3 == %d %d %d\n",*value,*light3Value,*light3Value2);
          char command[512];
          
            sprintf(command, "{ \"on\": true, \"hue\": 25500, \"sat\": 100, \"bri\": %d }",*light3Value2);
fprintf(stderr,"!!!! [%s]\n",command);
  		request = new HttpRequest(hue_request,(const char *)command);
               http.AddRequest(request);

        }
        case AF_LIGHT2BRI:
        {
          uint16_t *light3Value= (uint16_t *) value;
          uint8_t *light3Value2= (uint8_t *) value;
          sprintf(hue_request,"%slights/2/state",hue_prefix);
          fprintf(stdout,"AF_LIGHT3 == %d %d %d\n",*value,*light3Value,*light3Value2);
          char command[512];
          
            sprintf(command, "{ \"on\": true, \"hue\": 25500, \"sat\": 100, \"bri\": %d }",*light3Value2);
fprintf(stderr,"!!!! [%s]\n",command);
  		request = new HttpRequest(hue_request,(const char *)command);
               http.AddRequest(request);

        }
#endif
        case AF_LIGHT3BRI:
        {
          uint16_t *light3Value= (uint16_t *) value;
          uint8_t *light3Value2= (uint8_t *) value;
          sprintf(hue_request,"%slights/3/state",hue_prefix);
          fprintf(stdout,"AF_LIGHT3 == %d %d %d\n",*value,*light3Value,*light3Value2);
          char command[512];
          
            sprintf(command, "{ \"on\": true, \"hue\": 25500, \"sat\": 100, \"bri\": %d }",*light3Value2);
fprintf(stderr,"!!!! [%s]\n",command);
  		request = new HttpRequest(hue_request,(const char *)command);
               http.AddRequest(request);

        }
        break;
	case AF_LIGHT1LABEL:
          {
          char *light1Label= (char *) value;
          fprintf(stdout,"bAF_LIGHT1LABEL == %s %d\n",light1Label);
          }

	case AF_LIGHT2LABEL:
          {
          char *light2Label= (char *) value;
          fprintf(stdout,"bAF_LIGHT2LABEL == %s %d\n",light2Label);
          }

	case AF_LIGHT3LABEL:
          {
          char *light3Label= (char *) value;
          fprintf(stdout,"bAF_LIGHT3LABEL == %s %d\n",light3Label);
          }
        
  }
    if (theLib->setAttributeComplete(requestId, attributeId, valueLen, value) != afSUCCESS) {
        fprintf(stderr,"setAttributeComplete failed!");
    }

  
}
void onAttributeSetComplete_callback(const uint8_t requestId, const uint16_t attributeId, const uint16_t valueLen, const uint8_t *value)
{
  fprintf(stdout,"onAttributeSetComplete_callback: %d len: %d %s\n",attributeId,valueLen,value);
  fprintf(stdout,"onAttributeSetComplete_callback\n\n");
  fprintf(stdout,"onAttrSetComplete id: ");
  printf("%x ",attributeId);
    printf(" value: ");
    printf("%d\n",*value);
    switch (attributeId) {
        case AF_LIGHT1: 
           fprintf(stdout,"AF_LIGHT1 == %d\n",*value); 
        break; 
        case AF_LIGHT2: 
           fprintf(stdout,"AF_LIGHT1 == %d\n",*value);
        break;
        case AF_LIGHT3:
          {
          uint16_t *light3Value= (uint16_t *) value;
          fprintf(stdout,"AF_LIGHT1 == %d %d\n",*value,light3Value);
          }
        break;
	case AF_LIGHT1LABEL:
          {
          char *light1Label= (char *) value;
          fprintf(stdout,"cAF_LIGHT1LABEL == %s %d\n",light1Label);
          }

	case AF_LIGHT2LABEL:
          {
          char *light1Label= (char *) value;
          fprintf(stdout,"cAF_LIGHT2LABEL == %s %d\n",light1Label);
          }

	case AF_LIGHT3LABEL:
          {
          char *light1Label= (char *) value;
          fprintf(stdout,"cAF_LIGHT3LABEL == %s %d \n",light1Label);
          }


	break;
        default:
            break;
    }

}
#if 0
void setModuloLED(int on) {
    if (moduloLEDIsOn != on) {
        int16_t attrVal = on ? LED_ON : LED_OFF; // Modulo LED is active low
        if (theLib->setAttribute(AF_MODULO_LED, attrVal) != afSUCCESS) {
            printf("Could not set LED\n");
        }
        moduloLEDIsOn = on;
    }
}
#endif


#define HUE_STATE_NEED_IP 1
#define HUE_STATE_HAS_IP 2
int main(int argc, char *argv[])
{

  HueLightResponse *hueLightResponder= NULL;
  HueIPResponse *IPAddr= new HueIPResponse("https://www.meethue.com/api/nupnp");
  http.AddRequest(IPAddr);

  int state = HUE_STATE_NEED_IP;

        struct pollfd fdset[2];
        int nfds = 2;
        int gpio_fd, timeout, rc;
        char *buf[MAX_BUF];
        unsigned int interrupt_line= 17;/*17;*/
        unsigned int reset_line= 4;
        int len;
        int counter=0;

	Stream *theLog = new linuxLog();
        afSPI *theSPI = new linuxSPI();

        gpio_export(interrupt_line);
        gpio_set_dir(interrupt_line, 0);
        gpio_set_edge(interrupt_line, "falling");
        gpio_fd = gpio_fd_open(interrupt_line);

        timeout = POLL_TIMEOUT;

        fprintf(stdout,"Test\n");

        theLib = iafLib::create(0,isr_callback,onAttributeSet_callback,onAttributeSetComplete_callback,theLog,theSPI);
        theLib->mcuISR();

/* we need to hook up and use the reset line */
        gpio_export(reset_line);
        gpio_set_dir(reset_line, 1);
	gpio_set_value(reset_line,0);
    
        timespec sleep_time;
        timespec remaining;
        sleep_time.tv_sec=0;
        sleep_time.tv_nsec=250000;
        nanosleep(&sleep_time,&remaining);
         /* check for E_INTR? and call again? */
	gpio_set_value(reset_line,1);

        while (1) {
                counter++;
                memset((void*)fdset, 0, sizeof(fdset));

                fdset[0].fd = STDIN_FILENO;
                fdset[0].events = POLLIN;

                fdset[1].fd = gpio_fd;
                fdset[1].events = POLLPRI;

                lseek(gpio_fd, 0, SEEK_SET);    /* consume any prior interrupt */
                read(gpio_fd, buf, sizeof (buf));


                rc = poll(fdset, nfds, timeout);

                if (rc < 0) {
                        printf("\npoll() failed!\n");
                        return -1;
                }

                if (rc == 0) {
                        printf(".");
                }

                if (fdset[1].revents & POLLPRI) {
                        len = read(fdset[1].fd, buf, MAX_BUF);
                        printf("\npoll() GPIO %d interrupt occurred\n", interrupt_line);
                lseek(gpio_fd, 0, SEEK_SET);    /* consume interrupt */
                read(gpio_fd, buf, sizeof (buf));

                        theLib->mcuISR();
                }

                if (fdset[0].revents & POLLIN) {
                        (void)read(fdset[0].fd, buf, 1);
                        //printf("\npoll() stdin read 0x%2.2X\n", (unsigned int) buf[0]);
                }



		switch (state)
		{
		   case HUE_STATE_NEED_IP:
			// we need to check and see if we have an IP, then we can switch
  			if (IPAddr->completed())
{
                          state = HUE_STATE_HAS_IP;
                          IPAddr->get_ip(hue_ip);
                        fprintf(stderr,"IP: %s\n",hue_ip);
			sprintf(hue_prefix,"http://%s/api/huelibrary/",hue_ip);
                        state = HUE_STATE_HAS_IP;
}
			break;
		   case HUE_STATE_HAS_IP:

                // every so many loops check and start the lights query to update the lights..
		if (counter % 100 == 0)
                {
                   //check to see how many HTTP requests are outstanding
                   if (hueLightResponder == NULL)
                   { 
printf("****initializing new request to get the lights\n");
                      char hue_request[1024];
                      sprintf(hue_request,"%slights",hue_prefix);
                      hueLightResponder =  new HueLightResponse(hue_request,theLib);
                      http.AddRequest(hueLightResponder);
                   }
                   else if (hueLightResponder->completed())
                   {
                      delete hueLightResponder;
                      char hue_request[1024];
                      sprintf(hue_request,"%slights",hue_prefix);
                      hueLightResponder =  new HueLightResponse(hue_request,theLib);
                      http.AddRequest(hueLightResponder);
                   }
                  

                }

                }

  	http.Update();
  	http.Status();
        theLib->loop();
        fflush(stdout);
        }

    gpio_fd_close(gpio_fd);
   return 0;
}





int not_main(int argc, char *argv[])
{
  Http http;

//  HttpRequest *request1 = new HttpRequest("http://www.google.com");
  //http.AddRequest(request1);

//  HueLightResponse *request2 = new HueLightResponse("http://10.0.2.227/api/huelibrary/lights");
 // http.AddRequest(request2);

//HueLightResponse *IPAddr= new HueLightResponse("https://www.meethue.com/api/nupnp");
//http.AddRequest(IPAddr);

//  HttpRequest *request3 = new HueLightResponse("http://10.0.2.227/api/huelibrary/lights/1/state","{ \"on\": false, \"hue\": 25500, \"sat\": 100, \"bri\": 100 }");
 // http.AddRequest(request3);

  //HttpRequest *request = new HttpRequest("http://10.0.2.227/api/huelibrary/lights/3/state","{ \"on\": true, \"hue\": 25500, \"sat\": 100, \"bri\": 100 }");

//  http.AddRequest("http://www.google.com");
//  http.AddRequest("http://10.0.2.227/api/huelibrary/lights/3/state","{ \"on\": true, \"hue\": 25500, \"sat\": 100, \"bri\": 100 }");
  //http.AddRequest("http://10.0.2.227/");
 // http.AddRequest("http://10.0.2.227/api/huelibrary/lights");

  while (1)
  {
fprintf(stderr,"inside loop\n");
  // In some update loop called each frame
  http.Update();
  http.Status();
   sleep(1);
  }

 return 0;
}
