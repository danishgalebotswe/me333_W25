#include <stdio.h>
#include "io.h" // include the header file
#include "nu32dip.h"          // constants, funcs for startup and UART

int getUserInput(Investment *invp) {

  int valid;       
  char message[MAX_MESSAGE_LENGTH] = {}, msg[MAX_MESSAGE_LENGTH] = {}; // initialized message to print the start statement, msg to print out if valid input
  char user_input[MAX_MESSAGE_LENGTH] = {};

  // print out the prompt message
  sprintf(message, "Enter investment, growth rate, number of yrs (up to %d): \r\n", MAX_YEARS);
  NU32DIP_WriteUART1(message);

  // get the user input
  NU32DIP_ReadUART1(user_input, MAX_MESSAGE_LENGTH);
  NU32DIP_WriteUART1(user_input); 
  sscanf(user_input,"%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years)); 

  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
    (invp->years > 0) && (invp->years <= MAX_YEARS);
  sprintf(msg, "\n\rValid input?  %d\n\r",valid);
  NU32DIP_WriteUART1(msg); // print out if the input is valid or not

  
  if (!valid) {  
    NU32DIP_WriteUART1("Invalid input; exiting.\n\r"); // print out the input is invalid
  }
  return(valid);
} 


void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100];      

  NU32DIP_WriteUART1("\nRESULTS:\n\n\r");
  for (i=0; i<=yrs; i++) {  
    sprintf(outstring,"Year %3d:  %10.2f\n\r",i,arr[i]); 
    NU32DIP_WriteUART1(outstring); // print out the results from the program!!
  }
  NU32DIP_WriteUART1("\n\r");
} 