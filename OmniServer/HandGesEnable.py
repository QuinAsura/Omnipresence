#!/usr/bin/python

import socket, traceback
import os
import os.path
import time
import math
from wiringx86 import GPIOGalileoGen2 as GPIO
gpio = GPIO(debug=False)

import thread

#Stepper Motor Pins
step      = 0
direction = 1

#DC Motor Pins

#For Motor 1
dir1 = 2
pwm1 = 3
brk1 = 4

#For Motor 2
dir2 = 8
pwm2 = 9
brk2 = 10

pwmvalue = 50


#Pin Mode Settings for Stepper and DC Motors
gpio.pinMode(dir1, gpio.OUTPUT)
gpio.pinMode(pwm1, gpio.PWM)
gpio.pinMode(brk1, gpio.OUTPUT)

gpio.pinMode(dir2, gpio.OUTPUT)
gpio.pinMode(pwm2, gpio.PWM)
gpio.pinMode(brk2, gpio.OUTPUT)

gpio.pinMode(step, gpio.OUTPUT)
gpio.pinMode(direction, gpio.OUTPUT)

#Common To Sensor_Stream and Hand_Orientation_Stream
host = ''

#Port List
ss_port = 5550
ho_port = 5555
kc_port = 5500

#Sensor Binding to Port
sensor_s = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
hand_s = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
key_s = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)

sensor_s.setblocking(False)
hand_s.setblocking(False)
key_s.setblocking(False)

sensor_s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sensor_s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

key_s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
key_s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

hand_s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
hand_s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

sensor_s.bind((host,ss_port))
hand_s.bind((host,ho_port))
key_s.bind((host,kc_port))

#Data Initialisation
gyroX = ""
prev_message = "99"

def getSensorValue (Dummy):
	try:
		global gyroX
		message_s, address = sensor_s.recvfrom(8192)
		#print message_s
		gyro = message_s.split(",");
		gyroX = gyro[0]
		print "gyroX = " + gyroX
		sensor_s.flush()
	except:
		pass


def controlStepper (Dummy):
	try:
		global gyroX
		gyroX = float(gyroX)
		if gyroX > 0:
			#print "+ve Direction"
			gpio.digitalWrite(direction, gpio.HIGH)
		else:
			#print "-ve Direction"
			gpio.digitalWrite(direction, gpio.LOW)
	
		if abs(gyroX) > 0.05:
			stepperDelay  = 0.1
			gpio.digitalWrite(step, gpio.HIGH)
        		time.sleep(stepperDelay)
        		gpio.digitalWrite(step, gpio.LOW)
        		time.sleep(stepperDelay)
		else:
			gpio.digitalWrite(step, gpio.LOW)
	except:
		pass

				
def KeyUpdateData(Dummy):
	try:
		message_h, address = key_s.recvfrom(8192)
		
		if(message_h == "LEFT"):		
   			#Control Code for left turn
                        gpio.digitalWrite(dir2, gpio.HIGH)
                        gpio.analogWrite(pwm2,pwmvalue)
                        gpio.digitalWrite(brk2, gpio.LOW)
                        gpio.digitalWrite(dir1, gpio.LOW)
                        gpio.analogWrite(pwm1,0)
                        gpio.digitalWrite(brk1, gpio.HIGH)
                        time.sleep(0.3)
			gpio.analogWrite(pwm2,0)
                        #gpio.digitalWrite(brk2, gpio.HIGH)
                        print 'Left'
                        
		elif (message_h == "RIGHT"):
                        #Control Code for right turn
                        gpio.digitalWrite(dir2, gpio.LOW)
                        gpio.analogWrite(pwm2,pwmvalue)
                        gpio.digitalWrite(brk2, gpio.LOW)
                        gpio.digitalWrite(dir1, gpio.LOW)
                        gpio.analogWrite(pwm1,0)
                        gpio.digitalWrite(brk1, gpio.HIGH)
                        time.sleep(0.3)
			gpio.analogWrite(pwm2,0)
                        #gpio.digitalWrite(brk2, gpio.HIGH)
                        print 'Right'
    		elif (message_h == "UP"):
                        #Control code for Forward
                        gpio.digitalWrite(dir1, gpio.HIGH)
                        gpio.analogWrite(pwm1,pwmvalue)
                        gpio.digitalWrite(brk1, gpio.LOW)
                        gpio.digitalWrite(dir2, gpio.LOW)
                        gpio.analogWrite(pwm2,0)
                        gpio.digitalWrite(brk2, gpio.HIGH)
                        time.sleep(0.4)
			gpio.analogWrite(pwm1,0)	
                        #gpio.digitalWrite(brk1, gpio.HIGH)
                        print 'Fwd'

		elif (message_h == "DOWN"):
                        #Control code for Reverse
                        gpio.digitalWrite(dir1, gpio.LOW)
                        gpio.analogWrite(pwm1,pwmvalue)
                        gpio.digitalWrite(brk1, gpio.LOW)
                        gpio.digitalWrite(dir2, gpio.LOW)
                        gpio.analogWrite(pwm2,0)
                        gpio.digitalWrite(brk2, gpio.HIGH)
                        time.sleep(0.4)
			gpio.analogWrite(pwm1,0)
                        #gpio.digitalWrite(brk1, gpio.HIGH)
                        print 'Rev'
                else:
                        pass
                        
	except:
		pass

def HandUpdateData(Dummy):
	try:
		message_h, address = hand_s.recvfrom(8192)
		if (prev_message != message_h):
                        if(message_h == "LEFT"):		
                                #Control Code for left turn
                                gpio.digitalWrite(dir2, gpio.HIGH)
                                gpio.analogWrite(pwm2,pwmvalue)
                                gpio.digitalWrite(brk2, gpio.LOW)
                                gpio.digitalWrite(dir1, gpio.LOW)
                                gpio.analogWrite(pwm1,0)
                                gpio.digitalWrite(brk1, gpio.HIGH)
                                time.sleep(0.3)
                                gpio.analogWrite(pwm2,0)
                                #gpio.digitalWrite(brk2, gpio.HIGH)
                                print 'Left'
                                
                        elif (message_h == "RIGHT"):
                                #Control Code for right turn
                                gpio.digitalWrite(dir2, gpio.LOW)
                                gpio.analogWrite(pwm2,pwmvalue)
                                gpio.digitalWrite(brk2, gpio.LOW)
                                gpio.digitalWrite(dir1, gpio.LOW)
                                gpio.analogWrite(pwm1,0)
                                gpio.digitalWrite(brk1, gpio.HIGH)
                                time.sleep(0.3)
                                gpio.analogWrite(pwm2,0)
                                #gpio.digitalWrite(brk2, gpio.HIGH)
                                print 'Right'
                        elif (message_h == "UP"):
                                #Control code for Forward
                                gpio.digitalWrite(dir1, gpio.HIGH)
                                gpio.analogWrite(pwm1,pwmvalue)
                                gpio.digitalWrite(brk1, gpio.LOW)
                                gpio.digitalWrite(dir2, gpio.LOW)
                                gpio.analogWrite(pwm2,0)
                                gpio.digitalWrite(brk2, gpio.HIGH)
                                time.sleep(0.4)
                                gpio.analogWrite(pwm1,0)	
                                #gpio.digitalWrite(brk1, gpio.HIGH)
                                print 'Fwd'

                        elif (message_h == "DOWN"):
                                #Control code for Reverse
                                gpio.digitalWrite(dir1, gpio.LOW)
                                gpio.analogWrite(pwm1,pwmvalue)
                                gpio.digitalWrite(brk1, gpio.LOW)
                                gpio.digitalWrite(dir2, gpio.LOW)
                                gpio.analogWrite(pwm2,0)
                                gpio.digitalWrite(brk2, gpio.HIGH)
                                time.sleep(0.4)
                                gpio.analogWrite(pwm1,0)
                                #gpio.digitalWrite(brk1, gpio.HIGH)
                                print 'Rev'
                        else:
                                pass

                else:
                        pass
                        
	except:
		pass



while 1:
	try:
		thread.start_new_thread(getSensorValue,("Thread get GyroX",))
		thread.start_new_thread(controlStepper,("Thread Control Stepper",))
		
	except:	
		pass

	try:
                thread.start_new_thread(KeyUpdateData, ("Update Key Cnt",))
        except:
                pass

        try:
                thread.start_new_thread(HandUpdateData, ("Update Hand Data",))
        except:
                pass
