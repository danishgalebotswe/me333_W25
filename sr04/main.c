#include "sr04.h"
#include "sendDistance.h"
#include "nu32dip.h"

void delay(int time);

int main(){
    int timeout, coretick;
    float distance;

    SR04_Startup();
    
    timeout = 24000000;

    while (1) {
      coretick = SR04_read_raw(timeout);
      distance = SR04_read_meters(coretick);
      _CP0_SET_COUNT(0);                                               // wait at least 10us
      while (_CP0_GET_COUNT() < 0.25*24000000) {}
      sendOutput(distance);
    }
}

void delay(int time) {
  int j;
  for (j = 0; j < (time * 24000000); j++) { 
    }
}