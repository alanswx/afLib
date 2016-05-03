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
#include <unistd.h>

#include <jansson.h>
#include <stdio.h>
#include <mosquitto.h>

#include <map>
#include <iostream>

std::map<int,std::string> attrNameLookup;
std::map<std::string,int> attrLookup;
std::map<int,std::string> attrTypeLookup;


#include "gpiolib.h"

#include "linuxLog.h"
#include "linuxSPI.h"

struct mosquitto *mosq = NULL;
iafLib *theLib = NULL;

char global_id[1024];


void isr_callback()
{
  fprintf(stdout,"isr_callback\n");
}

void onAttributeSet_callback(const uint8_t requestId, const uint16_t attributeId, const uint16_t valueLen, const uint8_t *value)
{
  fprintf(stdout,"onAttributeSet_callback: %d len: %d %x %s\n",attributeId,valueLen,*value,value);

  char topic[1024];
  //sprintf(topic,"afero/%s/%d/%d",global_id,requestId,attributeId);
  std::string name = attrNameLookup[attributeId];
  sprintf(topic,"afero/%s/%s",global_id,name.c_str());
  std::string dataType = attrTypeLookup[attributeId];
char v[1024];

  if (!dataType.compare("BOOLEAN"))
	{
		int val = *value;
		if (val) strcpy(v,"true"); else strcpy(v,"false");
		int result = mosquitto_publish(mosq,0,topic,strlen(v),v,0,false);
		fprintf(stderr,"callback: %d topic %s\n",result,topic);
	} else if (!dataType.compare("UTF8S"))
	{

		int result = mosquitto_publish(mosq,0,topic,valueLen,value,0,false);
		fprintf(stderr,"callback: %d topic %s\n",result,topic);
	} else if (!dataType.compare("SINT16"))
	{
		sprintf(v,"%d",*value);		
		int result = mosquitto_publish(mosq,0,topic,strlen(v),v,0,false);
		fprintf(stderr,"callback: %d topic %s\n",result,topic);
	} else if (!dataType.compare("SINT64"))
	{
		sprintf(v,"%ld",*value);		
		int result = mosquitto_publish(mosq,0,topic,strlen(v),v,0,false);
		fprintf(stderr,"callback: %d topic %s\n",result,topic);
	}
	else
	{
		std::cerr << "dataType not found: "<< dataType << std::endl;
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

}

void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	if(message->payloadlen){
		printf("\n[[==%s %s\n", message->topic, message->payload);


	}else{
		printf("\n[[==%s (null)\n", message->topic);
	}
	fflush(stdout);

	std::string str(message->topic);
  	unsigned found = str.find_last_of("/");
        std::string path =  str.substr(0,found);
  	std::string var =  str.substr(found+1);

	std::cerr << "path [" << path << "] var [" << var << "]"<<std::endl;

        /* we need to lookup the mapping, and find the size */


	/* pull the topic apart */
        int attrID = attrLookup[var];
        if (attrID ==0)
        {
fprintf(stderr,"possible lookup error %s %d\n",message->topic,attrID);
        }
        std::string dataType = attrTypeLookup[attrID];

        if (!dataType.compare("BOOLEAN"))
	{
		int val = 0;
		if (!strcasecmp((char *)message->payload,"true"))
		{
			val = 1;
		}

		fprintf(stderr,"about to call SetAttributeBool with id %d val %d\n",attrID,val);
        	if (theLib->setAttributeBool(attrID, val) != afSUCCESS) {
            		printf("===Could not set Attribute\n");
        	}
	} else if (!dataType.compare("SINT16"))
	{
		int16_t val = atoi((char *)message->payload);
		fprintf(stderr,"about to call SetAttribute with id %d val %d\n",attrID,val);
        	if (theLib->setAttribute16(attrID, val) != afSUCCESS) {
            		printf("===Could not set Attribute\n");
        	}
	} else if (!dataType.compare("SINT64"))
	{
		int64_t val = atoll((char *)message->payload);
		fprintf(stderr,"about to call SetAttribute with id %d val %d\n",attrID,val);
        	if (theLib->setAttribute64(attrID, val) != afSUCCESS) {
            		printf("===Could not set Attribute\n");
        	}
	} else if (!dataType.compare("UTF8S"))
	{
		fprintf(stderr,"about to call SetAttribute with id %d val %s\n",attrID,(char *)message->payload);
        	if (theLib->setAttribute(attrID, strlen((char *)message->payload),(const char *)message->payload) != afSUCCESS) {
            		printf("===Could not set Attribute\n");
        	}
	}
	else
	{
		std::cerr << "dataType not found: "<< dataType << std::endl;
	}

}

void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
	int i;
	if(!result){
		/* Subscribe to broker information topics on successful connect. */
		//mosquitto_subscribe(mosq, NULL, "$SYS/#", 2);
                char sub[1024];
                sprintf(sub,"afero/%s/#",global_id);
		mosquitto_subscribe(mosq, NULL, sub, 2);
	}else{
		fprintf(stderr, "Connect failed\n");
	}
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
	int i;

	printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
	for(i=1; i<qos_count; i++){
		printf(", %d", granted_qos[i]);
	}
	printf("\n");
}

void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
	/* Pring all log messages regardless of level. */
	//printf("%s\n", str);
}



void createIDMapping(char *filename)
{
    json_t *json;
    json_error_t error;

    json = json_load_file(filename, 0, &error);

if (!json)
{
fprintf(stderr,"error text: %s\n",error.text);
fprintf(stderr,"error source: %s\n",error.source);
fprintf(stderr,"error line: %d\n",error.line);
fprintf(stderr,"error col: %d\n",error.column);
fprintf(stderr,"error position: %d\n",error.position);
}
    if (json)
    {
      json_t *services= json_object_get(json, "services");
      json_t *at0 = json_array_get(services,0);
      json_t *attributes= json_object_get(at0, "attributes");
      size_t index;
      json_t *value;

      json_array_foreach(attributes,index,value)
      {
         fprintf(stderr,"index = %d\n",index);
        
      	json_t *semanticType= json_object_get(value, "semanticType");
       	const char *semanticType_s=NULL;
       	const char *dataType_s=NULL;
       	int id_s = -1;
       	int size_s = -1;
      	if(json_is_string(semanticType))
      	{
       	  semanticType_s= json_string_value(semanticType);
       	  fprintf(stderr,"semanticType_s: [%s]\n",semanticType_s);
      	}	 
      	json_t *id= json_object_get(value, "id");
      	if(json_is_number(id))
      	{
       	  id_s = json_number_value(id);
       	  fprintf(stderr,"id_s: [%d]\n",id_s);
      	} 
      	json_t *dataType= json_object_get(value, "dataType");
      	if(json_is_string(dataType))
      	{
       	  dataType_s= json_string_value(dataType);
       	  fprintf(stderr,"dataType_s: [%s]\n",dataType_s);
      	}	 
        // save the entry into our hashmap
        if (id_s>=0 && semanticType_s && dataType_s)
        {
		attrNameLookup[id_s]=std::string(semanticType_s);
		attrLookup[semanticType_s]=id_s;
		attrTypeLookup[id_s]=dataType_s;
        }
      }
     
    } 

// services
// then loop the array - right now there is only 1?
// then get  attributes - an array
/*
                                        "id": 1,
                                        "dataType": "BOOLEAN",
                                        "semanticType": "Light1",
                                        "operations": [
                                                "READ",
                                                "WRITE"
                                        ],
                                        "defaultValue": "00",
                                        "length": 1
*/

}

int main(int argc, char *argv[])
{

	int i;
	const char *host = "localhost";
	int port = 1883;
	int keepalive = 60;
	bool clean_session = true;



        if (argc < 3)
        {
		fprintf(stderr,"usage: %s <devicename> <device-description.json>\n",argv[0]);
		exit(0);
        }

	strcpy(global_id,argv[1]);
	/* load the device-description json file so that we can convert the ID numbers into
           pretty strings for mosquitto
        */
	createIDMapping(argv[2]);

	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, clean_session, NULL);
	if(!mosq){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}
	mosquitto_log_callback_set(mosq, my_log_callback);
	mosquitto_connect_callback_set(mosq, my_connect_callback);
	mosquitto_message_callback_set(mosq, my_message_callback);
	mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);

	if(mosquitto_connect(mosq, host, port, keepalive)){
		fprintf(stderr, "Unable to connect.\n");
		return 1;
	}

// maybe we should use socket interface instead?
mosquitto_loop_start(mosq);

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



                  



        theLib->loop();
        fflush(stdout);
        }

    gpio_fd_close(gpio_fd);

        mosquitto_loop_stop(mosq,true);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

   return 0;
}





