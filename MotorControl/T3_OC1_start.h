#ifndef INTERRUPTSETUP_H
#define INTERRUPTSETUP_H

#include "xc.h"

void Timer3INT_start(void);
void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void);


#endif //TIMERSETUP_H