#include <xc.h>          // Load the proper header for the processor

#define MAXCYCLES 1000000 // define the maximum number of cycles
#define DELTACYCLES 100000 // define the difference in the cycles

int delay(int cycles);

int main(void) {
  TRISBCLR = 0x30; // Make TRIS for B4 and B5 0, 
  LATBbits.LATB4 = 0;    // Turn GREEN on and YELLOW off.  These pins sink current
  LATBbits.LATB5 = 1;    // on the NU32DIP, so "high" (1) = "off" and "low" (0) = "on"

  int cycles; // initialize integer cycles

  cycles = MAXCYCLES; // define cycles as MAXCYCLES at the beginning

  while(1) { 
    delay(cycles);
    LATBINV = 0b110000;    // toggle GREEN and YELLOW; 
    cycles -= DELTACYCLES;   // decrease cycles by DELTACYCLES

    if (cycles <= 0){     // cycles is less than 0, then redefine to MAXCYCLES
      cycles = MAXCYCLES; 
    }
  }
  return 0;
}

int delay(int cycles) {
  int j;
  for (j = 0; j < cycles; j++) { // number is 1 million
    while(!PORTAbits.RA4) {
        ;   // Pin A4 is the USER switch, low (FALSE) if pressed.
    }
  }
}
