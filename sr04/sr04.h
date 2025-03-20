#ifndef SR04__H__
#define SR04__H__

#define MAX_MESSAGE_LENGTH 200

void SR04_Startup();
unsigned int SR04_read_raw(int timeout);
float SR04_read_meters(int core_tick);
void NU32DIP_Startup();             // cache on, interrupts on, LED/button init, UART init



#endif // SR04__H__