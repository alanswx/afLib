
#include <string>
#include "iafLib.h"
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>

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
  fprintf(stdout,"Test\n");

  iafLib *theLib = iafLib::create(0,0,isr_callback,onAttributeSet_callback,onAttributeSetComplete_callback);
  for (int i=0;i<1000;i++)
  {
int reading = digitalRead (0) ;
if (reading==1) theLib->mcuISR();
   theLib->loop();
fprintf(stdout,"interrupt:%d\n",reading);

   sleep(1);
  // we may need to check for the interrupt here?
  }
return 0;
}
