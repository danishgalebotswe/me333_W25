#include "nu32dip.h"                      // constants, functions for startup and UART
#include "stdio.h"
#include "xc.h"
#include "Timer2Setup.h"

void Timer2setup(void) {
    T2CONbits.TCKPS = 0;                    // Timer2 prescaler N=1 (1:1)
    PR2 = 9600-1;                          // period = (PR3+1) * N * 12.5 ns = 30 us, 1 kHz
    TMR2 = 0;                               // initial TMR2 count is 0
    T2CONbits.ON = 1;
}
