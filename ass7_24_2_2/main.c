#include "nu32dip.h"          // constants, functions for startup and UART

int main(void) {
  NU32DIP_Startup();          // cache on, interrupts on, LED/button init, UART init

  T3CONbits.TCKPS = 0;     // Timer2 prescaler N=1 (1:1)
  PR3 = 2400-1;              // period = (PR3+1) * N * 12.5 ns = 30 us, 20 kHz
  TMR3 = 0;                // initial TMR3 count is 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  RPB3Rbits.RPB3R = 0b0101;
  OC1CONbits.OCTSEL = 1;  // Use Timer3 for OC1
  OC1RS = 1800;             // duty cycle = OC1RS/(PR3+1) = 75%
  OC1R = 1800;              // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer2
  OC1CONbits.ON = 1;       // turn on OC1

  /*_CP0_SET_COUNT(0);       // delay 4 seconds to see the 75% duty cycle on a 'scope
  while(_CP0_GET_COUNT() < 4 * 40000000) {
    ;
  }*/
  OC1RS = 1800;            // set duty cycle to 75%
  while(1) {
    ;                      // infinite loop
  }
  return 0;
}