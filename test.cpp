
#include <string>
#include "iafLib.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#include "gpiolib.h"
#include "examples/afBlink/profile/afBlink/device-description.h"

iafLib *theLib = NULL;

long lastBlink = 0;
bool blinking = false;
bool moduloLEDIsOn = false;      // Track whether the Modulo LED is on
uint16_t moduleButtonValue = 1;  // Track the button value so we know when it has changed

// Modulo LED is active low
#define LED_OFF                   1
#define LED_ON                    0


void isr_callback()
{
  fprintf(stdout,"isr_callback\n");
}
void onAttributeSet_callback(const uint8_t requestId, const uint16_t attributeId, const uint16_t valueLen, const uint8_t *value)
{
  fprintf(stdout,"onAttributeSet_callback: %d len: %d %x %s\n",attributeId,valueLen,*value,value);
  switch (attributeId)
  {
        case AF_BLINK:
          blinking = (*value==1);
          fprintf(stdout,"blinking == %d\n",blinking);
        break;
        
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
        // Update the state of the LED based on the actual attribute value.
        case AF_MODULO_LED:
            moduloLEDIsOn = (*value == 0);
            break;

            // Allow the button on Modulo to control our blinking state.
        case AF_MODULO_BUTTON: {
            uint16_t *buttonValue = (uint16_t *) value;
            if (moduleButtonValue != *buttonValue) {
                moduleButtonValue = *buttonValue;
                blinking = !blinking;
                if (theLib->setAttribute(AF_BLINK, blinking) != afSUCCESS) {
                    fprintf(stderr,"Could not set BLINK\n");
                }
            }
        }
            break;

        default:
            break;
    }

}
void setModuloLED(int on) {
    if (moduloLEDIsOn != on) {
        int16_t attrVal = on ? LED_ON : LED_OFF; // Modulo LED is active low
        if (theLib->setAttribute(AF_MODULO_LED, attrVal) != afSUCCESS) {
            printf("Could not set LED\n");
        }
        moduloLEDIsOn = on;
    }
}



void toggleModuloLED() {
    setModuloLED(!moduloLEDIsOn);
}


int main(int argc, char *argv[])
{

        struct pollfd fdset[2];
        int nfds = 2;
        int gpio_fd, timeout, rc;
        char *buf[MAX_BUF];
        unsigned int gpio = 17;/*17;*/
        int len;
        int counter=0;

        gpio_export(gpio);
        gpio_set_dir(gpio, 0);
        gpio_set_edge(gpio, "falling");
        gpio_fd = gpio_fd_open(gpio);

        timeout = POLL_TIMEOUT;

        fprintf(stdout,"Test\n");

        theLib = iafLib::create(2,0,isr_callback,onAttributeSet_callback,onAttributeSetComplete_callback);
        theLib->mcuISR();


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
                        printf("\npoll() GPIO %d interrupt occurred\n", gpio);
                lseek(gpio_fd, 0, SEEK_SET);    /* consume interrupt */
                read(gpio_fd, buf, sizeof (buf));

                        theLib->mcuISR();
                }

                if (fdset[0].revents & POLLIN) {
                        (void)read(fdset[0].fd, buf, 1);
                        printf("\npoll() stdin read 0x%2.2X\n", (unsigned int) buf[0]);
                }



                 if (blinking) {
                   if (counter % 20 ==0 ) {
                         toggleModuloLED();
                     }
                 } else {
                     setModuloLED(false);
                 }


                 theLib->loop();
                fflush(stdout);
        }

    gpio_fd_close(gpio_fd);
   return 0;
}




