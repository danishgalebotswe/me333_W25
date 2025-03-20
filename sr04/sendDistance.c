#include <stdio.h>
#include "sendDistance.h"
#include "nu32dip.h"

void sendOutput(float distance){
    char outstring[100];

    NU32DIP_Startup();

    sprintf(outstring, "The echo distance: %.4f \r\n", distance);
    NU32DIP_WriteUART1(outstring);
}