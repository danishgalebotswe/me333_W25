#include "nu32dip.h"                      // constants, functions for startup and UART
#include "stdio.h"
#include "xc.h"
#include "Timer4Setup.h"

void Timer4setup(void) {
    T4CONbits.TCKPS = 2;                    // Timer4 prescaler N=4 (4:1)
    PR4 = 60000-1;                          // period = (PR4+1) * N * 12.5 ns = 30 us, 200 Hz
    TMR4 = 0;                               // initial TMR4 count is 0
    T4CONbits.ON = 1;
}
