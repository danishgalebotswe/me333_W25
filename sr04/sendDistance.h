#ifndef SENDDISTANCE_H
#define SENDDISTANCE_H

#include "sr04.h"

void sendOutput(float distance);
void NU32DIP_Startup();             // cache on, interrupts on, LED/button init, UART init

#endif // SENDDISTANCE_H
