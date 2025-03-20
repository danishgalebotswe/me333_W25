# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import matplotlib.pyplot as plt 
from statistics import mean 

def read_plot_matrix():
    n_str = ser.read_until(b'\n').decode('utf-8').strip();  # get the number of data points to receive
    n_int = int(n_str) # turn it into an int
    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n').decode('utf-8').strip();  # get the data as a string, ints seperated by spaces
        dat_f = list(map(float,dat_str.split())) # now the data is a list
        ref.append(dat_f[1])
        data.append(dat_f[2])
        data_received = data_received + 1
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    plt.plot(t,ref,'r*-',t,data,'b*-')
    plt.title('Score = ' + str(score))
    plt.ylabel('value')
    plt.xlabel('index')
    plt.show()

from genref import genRef

import serial
ser = serial.Serial('COM9',230400)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\ta: Read current sensor (mA) \tf: Set PWM (-100 to 100) \tp: Unpower the motor \tg: Set current gains \th: Get current gains \tk: Test current gains \ti: Set position gains \tj: Get position gains \tl: Go to angle (deg) \tc: Read encoder (counts) \td: Read encoder (deg) \te: Reset encoder \tr: Get Mode \tm: Load step trajectory \tn: Load cubic trajectory \to: Execute trajectory \tx: Dummy 2 Command\tq: Quit') # '\t' is a tab
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'a'):
        
        current_str = ser.read_until(b'\n') # get the counts
        current_float = float(current_str) # turn into an int
        print('The current is ' + str(current_float) + ' mA\n') # print it to the screen to double check

    elif (selection == 'f'):
        
        print('PWM mode set!')
        PWM_str = input('What PWM value would you like [-100 to 100]? ') # get the PWM input
        PWM_int = int(PWM_str) # turn it into an int
        print('PWM value = ' + str(PWM_int)) # print it to the screen to double check

        ser.write((str(PWM_int)+'\n').encode()); # send the PWM

    elif (selection == 'p'):
        print('Unpowering the motor')

    elif (selection == 'g'):
        
        Kp_str = input('Enter your desired Kp current gain: ') # get the Kp input
        Kp_float = float(Kp_str) # turn it into an float

        ser.write((str(Kp_float)+'\n').encode()); # send the Kp

        Ki_str = input('Enter your desired Ki current gain: ') # get the Ki input
        Ki_float = float(Ki_str) # turn it into an float

        ser.write((str(Ki_float)+'\n').encode()); # send the Ki

        print('Sending Kp = ' + str(Kp_float) + ' and Ki = ' + str(Ki_float) + ' to the current controller.\n') # print it to the screen to double check

    elif (selection == 'h'):

        Kp_str = ser.read_until(b'\n').decode('utf-8').strip();  # get the Kp gain
        Kp_float = float(Kp_str) # turn it into an float

        Ki_str = ser.read_until(b'\n').decode('utf-8').strip();  # get the Ki gain
        Ki_float = float(Ki_str) # turn it into an float

        print('The current controller is using Kp = ' + str(Kp_float) + ' and Ki = ' + str(Ki_float) + '.\n') # print it to the screen

    elif (selection == 'k'):
        read_plot_matrix()
    
    elif (selection == 'i'):
        
        Kppos_str = input('Enter your desired Kp position gain: ') # get the Kp input
        Kppos_float = float(Kppos_str) # turn it into an float

        ser.write((str(Kppos_float)+'\n').encode()); # send the Kp

        Kipos_str = input('Enter your desired Ki position gain: ') # get the Ki input
        Kipos_float = float(Kipos_str) # turn it into an float

        ser.write((str(Kipos_float)+'\n').encode()); # send the Ki

        Kdpos_str = input('Enter your desired Kd position gain: ') # get the Ki input
        Kdpos_float = float(Kdpos_str) # turn it into an float

        ser.write((str(Kdpos_float)+'\n').encode()); # send the Kd

        print('Sending Kp = ' + str(Kppos_float) + ', Ki = ' + str(Kipos_float) + ' and Kd = ' + str(Kdpos_float) + ' to the position controller.\n') # print it to the screen to double check

    elif (selection == 'j'):

        Kppos_str = ser.read_until(b'\n').decode('utf-8').strip();  # get the Kp gain
        Kppos_float = float(Kppos_str) # turn it into an float

        Kipos_str = ser.read_until(b'\n').decode('utf-8').strip();  # get the Ki gain
        Kipos_float = float(Kipos_str) # turn it into an float

        Kdpos_str = ser.read_until(b'\n').decode('utf-8').strip();  # get the Ki gain
        Kdpos_float = float(Kdpos_str) # turn it into an float

        print('The position controller is using Kp = ' + str(Kppos_float) + ', Ki = ' + str(Kipos_float) + ' and Kd = ' +  str(Kdpos_float) + '.\n') # print it to the screen

    elif (selection == 'l'):
        
        Angle_str = input('Enter the desired motor angle in degrees: ') # get the angle input
        Angle_int = int(Angle_str) # turn it into an int
        print('Motor moving to ' + str(Angle_int) + ' degrees.') # print it to the screen to double check

        ser.write((str(Angle_int)+'\n').encode()); # send the angle
    
    elif (selection == 'c'):
        
        count_str = ser.read_until(b'\n') # get the counts
        count_int = int(count_str) # turn into an int
        print('The encoder count is ' + str(count_int) + ' counts\n') # print it to the screen to double check

    elif (selection == 'd'):
    
        deg_str = ser.read_until(b'\n') # get the deg
        deg_float = float(deg_str) # turn into an float
        print('The motor angle is ' + str(deg_float) + ' degrees\n') # print it to the screen to double check

    elif (selection == 'e'):
    
        print('Encoder Reset Complete!\n') # print it to the screen to double check

    elif (selection == 'r'):
    
        mode_str = ser.read_until(b'\n').decode('utf-8').strip() # get the mode
        print('The current mode is ' + mode_str + '.\n') # print it to the screen to double check
    
    elif (selection == 'm'):
        ref = genRef('step')
        #print(len(ref))
        t = range(len(ref))
        plt.plot(t,ref,'r*-')
        plt.ylabel('angle in degrees')
        plt.xlabel('index')
        plt.show()
        # send 
        ser.write((str(len(ref))+'\n').encode())
        for i in ref:
            ser.write((str(i)+'\n').encode())

        print("Reference points sent!")
    
    elif (selection == 'n'):
        ref = genRef('cubic')
        #print(len(ref))
        t = range(len(ref))
        plt.plot(t,ref,'r*-')
        plt.ylabel('angle in degrees')
        plt.xlabel('index')
        plt.show()
        # send 
        ser.write((str(len(ref))+'\n').encode())
        i_ind = 1
        for i in ref:
            ser.write((str(i)+'\n').encode())

        print("Reference points sent!")

    elif (selection == 'o'):
        read_plot_matrix()

    elif (selection == 'x'):
        # example operation
        n_str = input('Enter first number: ') # get the number to send
        n_int = int(n_str) # turn it into an int
        print('first number = ' + str(n_int)) # print it to the screen to double check

        ser.write((str(n_int)+'\n').encode()); # send the number

        # example operation
        p_str = input('Enter second number: ') # get the number to send
        p_int = int(p_str) # turn it into an int
        print('second number = ' + str(p_int)) # print it to the screen to double check

        ser.write((str(p_int)+'\n').encode()); # send the number
        
        sum_str = ser.read_until(b'\n');  # get the incremented number back
        sum_int = int(sum_str) # turn it into an int

        print('Got back: ' + str(sum_int) + '\n') # print it to the screen
        
    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()
    else:
        print('Invalid Selection ' + selection_endline)



