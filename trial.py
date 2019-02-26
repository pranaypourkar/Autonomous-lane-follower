import RPi.GPIO as GPIO
import time
import readchar
import signal
import sys
import xlsxwriter
import cv2



# Function to use ctrl+C on terminal
#def signal_handler(sig, frame):
#        print('You pressed Ctrl+C!')
#        p.stop()
#        GPIO.cleanup()
#        sys.exit(0)
    
    
    
GPIO.setmode(GPIO.BOARD)

GPIO.setup(12, GPIO.OUT)
GPIO.setup(11, GPIO.OUT)

p = GPIO.PWM(12, 50)
q = GPIO.PWM(11, 50)


duty=4.7
duty1=7.5

p.start(duty)
q.start(duty1)

wb = xlsxwriter.Workbook('sample-test.xlsx')
ws = wb.add_worksheet('my sheet')

# write the header in row 0, which is Excel row 1
ws.write_row(0, 0, ['angle'])
i=0;
vidcap = cv2.VideoCapture(0);
success,image = vidcap.read()
success = True

while success:
    
    x = readchar.readkey()
    #for servo
    if x=="a":
        duty1=duty1-0.25
    elif x=="d":
        duty1=duty1+0.25
    elif x=="s":
        duty1=7.5
 #to exit   
    if x=="e":
        wb.close()
        sys.exit(0)
        
 #for BLDC       
    if x=="w":
        duty = 5
    elif x==" ":
        duty = 4.7       
        
    if duty1 > 8.75:
        duty1 = 8.75
    if duty1 < 5.75:
        duty1 = 5.75
    
    q.ChangeDutyCycle(duty1)
    p.ChangeDutyCycle(duty)
    
    success,image = vidcap.read()
    #print 'Read a new frame: ', success ,
    cv2.imwrite("image/frame%d.png" % i, image)
    ws.write_row(i+1, 0, [duty1])
    i=i+1
    