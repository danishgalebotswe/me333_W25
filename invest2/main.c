#include "helper.h"           // all functions other than main

int main(void) {

  Investment inv;                

  while(getUserInput(&inv)) {     
    inv.invarray[0] = inv.inv0;  
    calculateGrowth(&inv);       
    sendOutput(inv.invarray,     
               inv.years);       
  }
  return 0;                      
} 