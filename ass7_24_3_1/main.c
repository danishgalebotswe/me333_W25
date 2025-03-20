#include "nu32dip.h"                      // constants, functions for startup and UART
#define NUMSAMPS 1000                     // number of points in waveform

static volatile int Waveform[NUMSAMPS];   // waveform

void makeWaveform();

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) { // _TIMER_2_VECTOR = 8
  static int counter = 0;                                 // initialize counter once

  OC1RS = Waveform[counter];  // insert line(s) to set OC1RS

  counter++;                                              // add one to counter every time ISR is entered
  if (counter == NUMSAMPS) {
    counter = 0;                                         // roll the counter over when needed
  }

  IFS0bits.T2IF = 0;                                       // insert line to clear interrupt flag
}

int main(void) {

  NU32DIP_Startup();                      // cache on, interrupts on, LED/button init, UART init

  __builtin_disable_interrupts(); // step 2: disable interrupts at CPU
  IPC2bits.T2IP = 5; // step 4: interrupt priority
  IPC2bits.T2IS = 0; // step 4: subp is 0, which is the default
  IFS0bits.T2IF = 0; // step 5: clear T2 interrupt flag
  IEC0bits.T2IE = 1; // step 6: enable core timer interrupt
  __builtin_enable_interrupts(); // step 7: CPU interrupts enabled

  T2CONbits.TCKPS = 0;     // Timer2 prescaler N=1 (1:1)
  PR2 = 48000-1;            // period = (PR3+1) * N * 12.5 ns = 30 us, 1 kHz
  TMR2 = 0;                 // initial TMR2 count is 0
  T2CONbits.ON = 1;

  T3CONbits.TCKPS = 0;                    // Timer3 prescaler N=1 (1:1)
  PR3 = 2400-1;                           // period = (PR3+1) * N * 12.5 ns = 30 us, 20 kHz
  TMR3 = 0;                               // initial TMR3 count is 0
  OC1CONbits.OCM = 0b110;                 // PWM mode without fault pin; other OC1CON bits are defaults
  RPB3Rbits.RPB3R = 0b0101;
  OC1CONbits.OCTSEL = 1;  // Use Timer3 for OC1
  OC1RS = 1800;             // duty cycle = OC1RS/(PR3+1) = 75%
  OC1R = 1800;              // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer3
  OC1CONbits.ON = 1;       // turn on OC1

  makeWaveform();

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

void makeWaveform() {
  int i = 0, center = (PR3+1)/2, A = (PR3+1)/4; // square wave, fill in center value and amplitude
  for (i = 0; i < NUMSAMPS; ++i) {
    if ( i < NUMSAMPS/2) {
      Waveform[i] = center + A;
    } else {
      Waveform[i] = center - A;
    }
  }
}