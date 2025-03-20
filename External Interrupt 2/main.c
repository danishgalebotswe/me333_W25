#include "nu32dip.h" // constants, funcs for startup and UART
#include <xc.h>

#define NUM_CYCLES 40000000;

volatile int timing = 0; // 0 = waiting, 1 = timing
volatile unsigned int init = 0, final = 0;
float diff; 

void __ISR(_EXTERNAL_2_VECTOR, IPL6SOFT) Ext2ISR(void) { // step 1: the ISR
    if (timing == 0) {
        init = _CP0_GET_COUNT();
        NU32DIP_WriteUART1("Press the USER button again to stop the timer!\r\n");
        timing = 1;
    } 

    else if (timing == 1) {
        char msg[100];
        final = _CP0_GET_COUNT();
        diff = final - init; // get the difference in time
        diff = (float)diff/NUM_CYCLES; // convert to seconds
        sprintf(msg, "Start time: %d Finish time: %d = %.3f seconds\r\n", init, final, diff);
        NU32DIP_WriteUART1(msg);
        NU32DIP_WriteUART1("Press the USER button to start the timer!\r\n");
        timing = 0;
    }
    
    
    // NU32_LED1 = 0; // LED1 and LED2 on
    // NU32_LED2 = 0;
    unsigned int j = _CP0_GET_COUNT();

    while (_CP0_GET_COUNT() < j + 600000) {;}  // delay for 400 k core ticks, 10 ms

    // NU32_LED1 = 1; // LED1 and LED2 off
    // NU32_LED2 = 1;
    IFS0bits.INT2IF = 0; // clear interrupt flag IFS0<3>
}


int main(void) {

    NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init

    __builtin_disable_interrupts(); // step 2: disable interrupts
    TRISBbits.TRISB6 = 1; //Set RB6 as input
    INT2Rbits.INT2R = 0b0001; // Assign INT2 to RB6

    INTCONbits.INT2EP = 0; // step 3: INT2 triggers on falling edge
    IPC2bits.INT2IP = 6; // step 4: interrupt priority 2
    IPC2bits.INT2IS = 0; // step 4: interrupt priority 1
    IFS0bits.INT2IF = 0; // step 5: clear the int flag
    IEC0bits.INT2IE = 1; // step 6: enable INT0 by setting IEC0<3>
    __builtin_enable_interrupts(); // step 7: enable interrupts

    NU32DIP_WriteUART1("Press the USER button to start the timer!\r\n");  // send the message to the user’s screen

    while(1) {;}

    return 0;
}