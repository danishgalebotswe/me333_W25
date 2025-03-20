#include "nu32dip.h" // config bits, constants, funcs for startuup nad UART
#include "stdio.h" // include any print functionality
#include "encoder.h" // include the encoder functionalities ( read and reset encoder)
#include "utilities.h" // include get and set mode functionality
#include "ina219.h" // communicate with the INA219 current sensor
#include <xc.h> 
#include "Timer2Setup.h"
#include "T3_OC1_start.h"
#include "i2c_master_noint.h" // using I2C communication
#include "Timer4Setup.h"
#include "math.h"

//include other header files here

#define BUF_SIZE 200
#define NUMSAMPS 1000
#define ITEST_NUMAMPS 100
#define PLOTPTS 100                       // number of data points to plot
#define TRACK_PLOTPTS 1000
#define EINTMAX 100

volatile int PWM_val;
volatile float angle_val;
static volatile float current_ref;
static volatile float Kp = 0, Ki = 0;     // current control gains
static volatile float Kppos = 0, Kipos = 0, Kdpos = 0;     // position control gains
static volatile int Waveform[ITEST_NUMAMPS];
static int center = 0; //  center of the waveform
volatile int A = 200; // amplitude of the waveform
static volatile int INAarray[PLOTPTS];    // measured values to plot
static volatile float ANGLEarray[TRACK_PLOTPTS];    // measured values to plot
static volatile int REFarray[PLOTPTS];    // reference values to plot
static volatile float TRACK_REFarray[TRACK_PLOTPTS];    // reference values to plot
static volatile float trajectory_REFarray[TRACK_PLOTPTS];
static volatile int StoringData = 0;      // if this flag = 1, currently storing
static volatile int i = 0;
static volatile int eint=0;                  // integral of the control error
static volatile int eprev=0;                  // previous of the control error
static volatile int edot=0;                  // derivative of the control error
static volatile float eposint=0;                  // integral of the control error
static volatile float eposprev=0;                  // previous of the control error
static volatile float eposdot=0;                  // derivative of the control error
static volatile int itest_counter = 0; 

float trajectory[200];
static volatile int trajectory_N = 0;

float PI_controller(int ref_waveform, float ina_val);
float PID_controller(float angle_val1, float encoder_val1);

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {   // _TIMER_2_VECTOR = 8
    static int counter = 0;
    static int plotind = 0;
    //static int decctr = 0; 
    static volatile float u;
    static float inaval = 0;
  
                                  // insert line(s) to set OC1RS
    //_CP0_SET_COUNT(0);
    //while (_CP0_GET_COUNT() < 24000){;}
  
  
    //OC1RS = 600; // duty cycle = OC1RS/(PR3+1) = 25%

    switch (get_mode()) {
        case IDLE:
        {
            OC1RS = 0;
            break;
        }

        case PWM:
        {
            if (PWM_val < -100) {
                PWM_val = -100;
            }
            else if (PWM_val > 100)
            {
                PWM_val = 100;}

            if (PWM_val > 0) {
                LATBbits.LATB6 = 0; // set forward direction
            }

            else if (PWM_val < 0) {
                LATBbits.LATB6 = 1; // set the oppposite direction
                //PWM_val = -PWM_val;
            }

            OC1RS = (int)((PWM_val/100.0) * PR3);

            break;
        }

        case ITEST:
        {
            if (itest_counter == 25) {
                A = -200;
            }

            else if (itest_counter == 50) {
                A = 200;
            }

            else if (itest_counter == 75) {
                A = -200;
            }

            else if (itest_counter == 99) {
                set_mode(IDLE);
                StoringData = 0;
            }

            Waveform[itest_counter] = center + A;
            REFarray[itest_counter] = Waveform[itest_counter];

            inaval = INA219_read_current();
            INAarray[itest_counter] = inaval;

            u = PI_controller(Waveform[itest_counter], inaval);

            if (itest_counter == 99) {
                A = 200;
            }

            itest_counter++;

            if (u > 100.0) {
                u = 100.0;
            } 
            else if (u < -100.0) {
                u = -100.0;
            }

            if (u > 0.0) {
                LATBbits.LATB6 = 0; // set forward direction
            }

            else if (u < 0.0) {
                LATBbits.LATB6 = 1; // set the oppposite direction
                u = -u;
            }
                
            OC1RS = (int)(u/100.0) * PR3;


            counter++;                                              // add one to counter every time ISR is entered
            if (counter == NUMSAMPS) {
                counter = 0;                                          // roll the counter over when needed
            }

            break;
        }

        case HOLD:
        {

            // if (current_ref > 100.0) {
            //     current_ref = 100.0;
            // } 
            // else if (current_ref < -100.0) {
            //     current_ref = -100.0;
            // }
            inaval = INA219_read_current();

            u = PI_controller(current_ref, inaval);

            if (u > 100.0) {
                u = 100.0;
            } 
            else if (u < -100.0) {
                u = -100.0;
            }

            if (u > 0.0) {
                LATBbits.LATB6 = 0; // set forward direction
            }

            else if (u < 0.0) {
                LATBbits.LATB6 = 1; // set the oppposite direction
                u = -u;
            }
                
            OC1RS = (int)((u/100.0) * PR3);

            break;
        }

        case TRACK:
        {

            // if (current_ref > 100.0) {
            //     current_ref = 100.0;
            // } 
            // else if (current_ref < -100.0) {
            //     current_ref = -100.0;
            // }
            inaval = INA219_read_current();

            u = PI_controller(current_ref, inaval);

            if (u > 100.0) {
                u = 100.0;
            } 
            else if (u < -100.0) {
                u = -100.0;
            }

            if (u > 0.0) {
                LATBbits.LATB6 = 0; // set forward direction
            }

            else if (u < 0.0) {
                LATBbits.LATB6 = 1; // set the oppposite direction
                u = -u;
            }
                
            OC1RS = (int)((u/100.0) * PR3);

            break;
        }
    }

  
    IFS0bits.T2IF = 0;                                       // insert line to clear interrupt flag
}

void __ISR(_TIMER_4_VECTOR, IPL6SOFT) Controller4(void) {   // _TIMER_2_VECTOR = 8
    static volatile int encoder_val;
    static int counter = 0;
    static int plotind = 0; 
    static volatile float upos;

                                  // insert line(s) to set OC1RS
    //_CP0_SET_COUNT(0);
    //while (_CP0_GET_COUNT() < 24000){;}
  
  
    //OC1RS = 600; // duty cycle = OC1RS/(PR3+1) = 25%

    switch (get_mode()) {
        case HOLD:
        {
            WriteUART2("a");
            while(!get_encoder_flag()){/*NU32DIP_WriteUART1("Waiting for flag\r\n");*/}\
            set_encoder_flag(0);
            encoder_val = (get_encoder_count()/1336.0)*360.0;

            upos = PID_controller(angle_val, encoder_val);
                
            //OC1RS = ((int) abs(u)/100.0) * PR3;

            current_ref = upos;

            break;
        }
    

    case TRACK:
        {   
            WriteUART2("a");
            while(!get_encoder_flag()){/*NU32DIP_WriteUART1("Waiting for flag\r\n");*/}\
            set_encoder_flag(0);

            encoder_val = (get_encoder_count()/1336.0)*360.0;

            TRACK_REFarray[plotind] = trajectory[plotind];
            ANGLEarray[plotind] = encoder_val;

            upos = PID_controller(trajectory[plotind], encoder_val);
                
            //OC1RS = ((int) abs(u)/100.0) * PR3;

            current_ref = upos;

            plotind++;

            if (plotind >= TRACK_PLOTPTS) {
                set_mode(HOLD);
                StoringData = 0;
                plotind = 0;
            }

            counter++;                                              // add one to counter every time ISR is entered
            if (counter == NUMSAMPS) {
                counter = 0;                                          // roll the counter over when needed
            }

            break;
        }
    }
  
    IFS0bits.T4IF = 0;                                       // insert line to clear interrupt flag
    
    LATBINV = (1 << 12);

}

int main(){
    float kptemp = 0, kitemp = 0;           // temporary local current gains
    float kppostemp = 0, kipostemp = 0, kdpostemp = 0;           // temporary local gains
    char buffer[BUF_SIZE];
    char message[BUF_SIZE];
    NU32DIP_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
    UART2_Startup(); // initialize the encoder reading
    INA219_Startup(); // current sensing startup

    __builtin_disable_interrupts();                                        // keep ISR disabled as briefly as possible
    Kp = kptemp;                                                           // copy local variables to globals used by ISR
    Ki = kitemp;
    Kppos = kppostemp;                                                           // copy local variables to globals used by ISR
    Kipos = kipostemp;
    Kdpos = kdpostemp;
    __builtin_enable_interrupts(); 
    
    set_mode(IDLE);

    NU32DIP_WriteUART1("NU32 started!\r\n");

    __builtin_disable_interrupts();
    IPC2bits.T2IP = 5;                      // step 4: interrupt priority
    IPC2bits.T2IS = 0;                      // step 4: subp is 0, which is the default
    IFS0bits.T2IF = 0;                      // step 5: clear T2 interrupt flag
    IEC0bits.T2IE = 1;                      // step 6: enable core timer interrupt// in future, initializa modules or peripherals here
    
    IPC4bits.T4IP = 6;                      // step 4: interrupt priority
    IPC4bits.T4IS = 0;                      // step 4: subp is 0, which is the default
    IFS0bits.T4IF = 0;                      // step 5: clear T2 interrupt flag
    IEC0bits.T4IE = 1;                    // step 6: enable core timer interrupt// in future, initializa modules or peripherals here
    __builtin_enable_interrupts();

    Timer2setup();
    Timer3INT_start();
    Timer4setup();

    // Directional bit B6
    TRISBbits.TRISB6 = 0; // set B6 to output for the direction bit

    TRISBbits.TRISB12 = 0;
    LATBbits.LATB12 = 0;

    while (1){
        NU32DIP_ReadUART1(buffer,BUF_SIZE); // we expect the nxt character to be a menu command
        switch (buffer[0]) {
            case 'a':  // Read the current sensor (mA)
            {
                char m[50];
                float currentRead = INA219_read_current();
                sprintf(m,"%.2f\r\n",currentRead);
                NU32DIP_WriteUART1(m);
                break;
            }
            
            case 'f':  // Set PWM (-100 to 100)
            {

                NU32DIP_ReadUART1(message,BUF_SIZE); // get n
                sscanf(message,"%d",&PWM_val);

                set_mode(PWM);

                break;
            }

            case 'g':  // Set current gains . 
            {
                // NU32DIP_WriteUART1("Enter your desired Kp current gain: \r\n");
                NU32DIP_ReadUART1(buffer,BUF_SIZE);
                sscanf(buffer,"%f\r\n", &Kp);

                //NU32DIP_WriteUART1("Enter your desired Ki current gain: \r\n");
                NU32DIP_ReadUART1(buffer,BUF_SIZE);
                sscanf(buffer,"%f\r\n", &Ki);

                //sprintf(buffer, "Sending Kp = %.2f and Ki = %.2f to the current controller.\r\n", Kp, Ki);
                //NU32DIP_WriteUART1(buffer);
                break;
            }

            case 'h':  // Get current gains
            {
                sprintf(buffer, "%.2f\r\n", Kp); // set the Kp
                NU32DIP_WriteUART1(buffer);

                sprintf(buffer, "%.2f\r\n", Ki); // set the Ki
                NU32DIP_WriteUART1(buffer);
                break;
            }

            case 'k': // Test current gains
            {
                sprintf(message, "%d\r\n", PLOTPTS);
                NU32DIP_WriteUART1(message);

                set_mode(ITEST);
                StoringData = 1;                                                         // message to ISR to start storing data
                while (StoringData) {                                                    // wait until ISR says data storing is done
                ;                                                                      // do nothing
                }

                if (itest_counter == 100) {
                    itest_counter = 0;
                }

                for (i=0; i<PLOTPTS; i++) {                                              // send plot data to MATLAB
                                                                                        // when first number sent = 1, MATLAB knows we’re done
                sprintf(message, "%d %d %d\r\n", i, INAarray[i], REFarray[i]);
                NU32DIP_WriteUART1(message);
                }
                break;
            }
            
            case 'p':  // Unpower the motor
            {
                set_mode(IDLE);
            
                break;
            }
            case 'c':  // Read the encoder (counts)
            {
                WriteUART2("a");
                while(!get_encoder_flag()){/*NU32DIP_WriteUART1("Waiting for flag\r\n");*/}\
                set_encoder_flag(0);
                char m[50];
                int p = get_encoder_count();
                sprintf(m,"%d\r\n",p);
                NU32DIP_WriteUART1(m);
                break;
            }

            case 'd':  // Read the encoder (deg)
            {
                WriteUART2("a");
                while(!get_encoder_flag()){/*NU32DIP_WriteUART1("Waiting for flag\r\n");*/}\
                set_encoder_flag(0);
                char m[50];
                int p = get_encoder_count(); 
                float encoder_angle = (float)p/1336 * 360;
                sprintf(m,"%.2f\r\n", encoder_angle) ; // (counts * 360 / 1336) % 360 + 360) % 360
                NU32DIP_WriteUART1(m);
                break;
            }

            case 'e':  // Reset the encoder count to 32,768
            {
                WriteUART2("b"); // print ‘b’ from the PIC to the Pico, the Pico will reset the encoder count to 0 and will not reply.
                //while(!get_encoder_flag()){/*NU32DIP_WriteUART1("Waiting for flag\r\n");*/}\
                //set_encoder_flag(0);
                //char m[50];
                //int p = get_encoder_count();
                //sprintf(m,"%d\r\n",p);
                //NU32DIP_WriteUART1(m);
                //NU32DIP_WriteUART1("Reset Complete!\r\n");
                break;
            }

            case 'i':  // Set position gains. 
            {
                //NU32DIP_WriteUART1("Enter your desired Kp position gain: \r\n");
                NU32DIP_ReadUART1(buffer,BUF_SIZE);
                sscanf(buffer,"%f\r\n", &Kppos);

                //NU32DIP_WriteUART1("Enter your desired Ki position gain: \r\n");
                NU32DIP_ReadUART1(buffer,BUF_SIZE);
                sscanf(buffer,"%f\r\n", &Kipos);

                //NU32DIP_WriteUART1("Enter your desired Kd position gain: \r\n");
                NU32DIP_ReadUART1(buffer,BUF_SIZE);
                sscanf(buffer,"%f\r\n", &Kdpos);

                //sprintf(buffer, "Sending Kp = %.2f, Ki = %.2f and Kd = %.2f to the position controller.\r\n", Kppos, Kipos, Kdpos);
                //NU32DIP_WriteUART1(buffer);
                break;
            }

            case 'j':  // Get position gains
            {
                sprintf(buffer, "%.2f\r\n", Kppos); // get the Kp
                NU32DIP_WriteUART1(buffer);

                sprintf(buffer, "%.2f\r\n", Kipos); // get the Ki
                NU32DIP_WriteUART1(buffer);

                sprintf(buffer, "%.2f\r\n", Kdpos); // get the Ki
                NU32DIP_WriteUART1(buffer);

                //sprintf(buffer, "The position controller is using Kp = %.2f, Ki = %.2f, and Kd = %.2f.\r\n", Kppos, Kipos, Kdpos);
                //NU32DIP_WriteUART1(buffer);

                break;
            }

            case 'l':  // Go to angle (deg)
            {
                NU32DIP_ReadUART1(message,BUF_SIZE); // get the desire angle
                sscanf(message,"%f",&angle_val);

                // sprintf(buffer,"%.2f\r\n", angle_val);
                // NU32DIP_WriteUART1(buffer);

                set_mode(HOLD);

                break;
            }

            case 'm':  // Load step trajectory
            {
                int arrayind = 0;

                //NU32DIP_WriteUART1("Enter step trajectory, in sec and degrees [time1, ang1; time2, ang2; ...]: \r\n");
                NU32DIP_ReadUART1(buffer,BUF_SIZE);
                sscanf(buffer,"%d\r\n", &trajectory_N);
                
                while (arrayind < trajectory_N) {
                    NU32DIP_ReadUART1(buffer,BUF_SIZE);
                    sscanf(buffer,"%f\r\n", &trajectory_REFarray[arrayind]);
                    arrayind++;
                }

                break;
            }

            case 'n':  // Load cubic trajectory
            {

                //NU32DIP_WriteUART1("Enter cubic trajectory, in sec and degrees [time1, ang1; time2, ang2; ...]: \r\n");
                int arrayind = 0;

                //NU32DIP_WriteUART1("Enter step trajectory, in sec and degrees [time1, ang1; time2, ang2; ...]: \r\n");
                NU32DIP_ReadUART1(buffer,BUF_SIZE);
                sscanf(buffer,"%d\r\n", &trajectory_N);

                
                while (arrayind < trajectory_N) {
                    NU32DIP_ReadUART1(buffer,BUF_SIZE);
                    sscanf(buffer,"%f\r\n", &trajectory_REFarray[arrayind]);
                    arrayind++;
                }
                //sprintf(buffer, "Sending Kp = %.2f, Ki = %.2f and Kd = %.2f to the position controller.\r\n", Kppos, Kipos, Kdpos);
                //NU32DIP_WriteUART1(buffer);
                break;
            }

            case 'o':  // Load cubic trajectory
            {
                eint = 0;
                eposint = 0;

                sprintf(message, "%d\r\n", TRACK_PLOTPTS);
                NU32DIP_WriteUART1(message);

                set_mode(TRACK);
                StoringData = 1;                                                         // message to ISR to start storing data
                while (StoringData) {                                                    // wait until ISR says data storing is done
                //NU32DIP_WriteUART1("Here");                                                                      // do nothing
                }

                for (i=0; i<TRACK_PLOTPTS; i++) {                                              // send plot data to MATLAB
                                                                                        // when first number sent = 1, MATLAB knows we’re done
                sprintf(message, "%.2f %.2f %.2f\r\n", i, ANGLEarray[i], TRACK_REFarray[i]);
                NU32DIP_WriteUART1(message);
                }
                break;
            }

            case 'r':  // query the mode 
            {
                mode_t currentMode = get_mode();
                char m[50];
                sprintf(m,"%s\r\n",mode_t_2_str(currentMode));
                NU32DIP_WriteUART1(m);
                break;
            }

            case 'x':  // dummy command for demonstration purposes
            {
                int n = 0;
                int p = 0;

                NU32DIP_ReadUART1(buffer,BUF_SIZE); // get n
                sscanf(buffer,"%d",&n);

                NU32DIP_ReadUART1(buffer,BUF_SIZE); // get p
                sscanf(buffer,"%d",&p);

                sprintf(buffer,"%d\r\n",n+p); //return the n + p
                NU32DIP_WriteUART1(buffer);
                break;
            }
            case 'q':
            {
                set_mode(IDLE); //handle q for quit. Later you may want to return to IDLE mode here.
                break;
            }

            default:
            {
                NU32DIP_WriteUART1("Error!!!\r\n"); // indicate an error
                break;
            }
        }
    }

    return 0;
}


float PI_controller(int ref_waveform, float ina_val) {
    int e = ref_waveform - ina_val;
    eint = eint + e;
    float u = Kp*e + Ki*eint;
  
    return u;
}

float PID_controller(float angle_val1, float encoder_val1) {
    float epos = angle_val1 - encoder_val1;
    eposdot = epos - eposprev;
    eposint = eposint + epos;
    if (eposint > EINTMAX) {
        eposint = EINTMAX;
    }
    else if (eposint < -EINTMAX) {
        eposint = -EINTMAX;
    }

    float upos = Kppos*epos + Kipos*eposint + Kdpos*eposdot;
    eposprev = epos;
  
    return upos;
}
