
#include <string>
#include "iafLib.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#include <wiringPi.h>
#include "gpiolib.h"

void isr_callback()
{
  fprintf(stdout,"isr_callback\n");
}
void onAttributeSet_callback(const uint8_t requestId, const uint16_t attributeId, const uint16_t valueLen, const uint8_t *value)
{
  fprintf(stdout,"onAttributeSet_callback\n");
}
void onAttributeSetComplete_callback(const uint8_t requestId, const uint16_t attributeId, const uint16_t valueLen, const uint8_t *value)
{
  fprintf(stdout,"onAttributeSet_callback\n\n");
}

int main(int argc, char *argv[])
{

        struct pollfd fdset[2];
        int nfds = 2;
        int gpio_fd, timeout, rc;
        char *buf[MAX_BUF];
        unsigned int gpio = 17;/*17;*/
        int len;

#if 1
        gpio_export(gpio);
        gpio_set_dir(gpio, 0);
        gpio_set_edge(gpio, "falling");
        gpio_fd = gpio_fd_open(gpio);
#endif
        timeout = POLL_TIMEOUT;

  fprintf(stdout,"Test\n");

  iafLib *theLib = iafLib::create(2,0,isr_callback,onAttributeSet_callback,onAttributeSetComplete_callback);
theLib->mcuISR();
   theLib->mcuISR();
        while (1) {
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

#if 0
unsigned int val;
gpio_get_value(0,&val);
fprintf(stdout,"interrupt:%d\n",val);
if (val) theLib->mcuISR();
#endif

   theLib->loop();
                fflush(stdout);
        }

        gpio_fd_close(gpio_fd);
 // for (int i=0;i<1000000;i++)
 // {
//int reading = digitalRead (0) ;
//if (reading==1) theLib->mcuISR();
//fprintf(stdout,"interrupt:%d\n",reading);
//theLib->mcuISR();

   //sleep(1);
//   usleep(1);
  // we may need to check for the interrupt here?
 // }
#if 0
for (int i=0;i<1000000;i++)
{
   usleep(10);
   theLib->loop();
   theLib->mcuISR();
   theLib->loop();
  } 
#endif
return 0;
}




