#include "nu32dip.h"                      // constants, functions for startup and UART
#include "stdio.h"
#include "xc.h"

void Timer3INT_start(void) {
    T3CONbits.TCKPS = 0;                    // Timer3 prescaler N=1 (1:1)
    PR3 = 2400-1;                           // period = (PR3+1) * N * 12.5 ns = 30 us, 20 kHz
    TMR3 = 0;                               // initial TMR3 count is 0
    OC1CONbits.OCM = 0b110;                 // PWM mode without fault pin; other OC1CON bits are defaults
    RPB7Rbits.RPB7R = 0b0101;
    OC1CONbits.OCTSEL = 1;  // Use Timer3 for OC1

    OC1RS = 0;             // duty cycle = OC1RS/(PR3+1) = 75%
    OC1R = 1800;              // initialize before turning OC1 on; afterward it is read-only
    T3CONbits.ON = 1;        // turn on Timer3
    OC1CONbits.ON = 1;       // turn on OC1

    OC1RS = 0;
}

