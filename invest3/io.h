#ifndef IO_H
#define IO_H

#define MAX_YEARS 100   // define the maximum number of years
#define MAX_MESSAGE_LENGTH 200 // define the string length

typedef struct {
  double inv0;    // initial investment                
  double growth;  // growth rate                
  int years;       // number of years               
  double invarray[MAX_YEARS+1];   
} Investment;

int getUserInput(Investment *invp);
void sendOutput(double *arr, int years);
void NU32DIP_Startup();             // cache on, interrupts on, LED/button init, UART init

#endif // IO_H