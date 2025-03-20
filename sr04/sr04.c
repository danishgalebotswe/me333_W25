#include "sr04.h"
#include <xc.h>
#include "nu32dip.h"

// macros for the pins
#define TRIG LATBbits.LATB15
#define ECHO PORTBbits.RB14

// initialize the pins used by the SR04
void SR04_Startup(){
    ANSELA = 0; // turn off the analog input functionality that overrides everything else
    ANSELB = 0;
    TRISBbits.TRISB15 = 0; // B15 is TRIG, output from the PIC
    TRISBbits.TRISB14 = 1; // B14 is ECHO, input to the PIC
    TRIG = 0; // initialize TRIG to LOW
}

// trigger the SR04 and return the value in core timer ticks
unsigned int SR04_read_raw(int timeout){
    
    NU32DIP_Startup();

    int init, final;

    TRIG = 1;                                                        // turn on TRIG 

    _CP0_SET_COUNT(0);                                               // wait at least 10us
    while (_CP0_GET_COUNT() < 240) {}

    TRIG = 0;                                                        // turn off TRIG
    
    while (!ECHO) {}                                                 // wait until ECHO is on 
                                                        
    init = _CP0_GET_COUNT();                                         // note the value of the core timer

    while (ECHO || _CP0_GET_COUNT() > timeout) {}                    // wait until ECHO is off or timeout core ticks has passed

    final = _CP0_GET_COUNT();                                        // note the core timer

    return (final - init);                                           // return the difference in core times
}

float SR04_read_meters(int core_tick){
    float echo_distance;                                                  // read the raw rs04 value
    echo_distance = ((float) core_tick/24000000) * 0.5 * 346;;                          // convert the time to meters, the velocity of sound in air is 346 m/s
    return echo_distance;                                                 // return the distance in meters
}
