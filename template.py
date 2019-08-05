#!/usr/bin/python3
"""
Python Practical Template
Keegan Crankshaw
Readjust this Docstring as follows:
Names: David Sturrock
Student Number: STRDAV029
Prac: 1
Date: 29/07/2019
"""

# import Relevant Librares
import RPi.GPIO as GPIO
import itertools
import time
#Global Vars
global index
index=0
# Setup
GPIO.setmode(GPIO.BCM) #BCM pinmode chosen
#OUTPUT SETUP
GPIO.setup(17, GPIO.OUT)
GPIO.setup(23, GPIO.OUT)
GPIO.setup(24, GPIO.OUT)
GPIO.output(17,GPIO.LOW)
GPIO.output(23,GPIO.LOW)
GPIO.output(24,GPIO.LOW)
#INPUT SETUP
GPIO.setup(26, GPIO.IN,pull_up_down=GPIO.PUD_UP)
GPIO.setup(16, GPIO.IN,pull_up_down=GPIO.PUD_UP)
#ARRAY SETUP
list = [0,1,2,3,4,5,6,7,8]
x = 0

digit1 = [0,0,0,0,0,0,0,0]
digit2 = [0,0,0,0,0,0,0,0]
digit3 = [0,0,0,0,0,0,0,0]
for i in itertools.product([0,1],repeat=3):
    list[x]=i
    digit1[x]=list[x][0]
    digit2[x]=list[x][1]
    digit3[x]=list[x][2]
    x+=1
# FUNCTIONS
def increment(channel):
	if index==7 : 
		global index
		index=0
	else:  index+=1
	GPIO.output(17,digit1[index])
	GPIO.output(23,digit2[index])
	GPIO.output(24,digit3[index])

def decrement(channel) :
	if index==0 : 
		global index
		index=7
	else: index-=1
	GPIO.output(17,digit1[index])
        GPIO.output(23,digit2[index])
        GPIO.output(24,digit3[index])
#INTERRUPTS
GPIO.add_event_detect(26, GPIO.FALLING, callback=increment, bouncetime=300)
GPIO.add_event_detect(16, GPIO.FALLING, callback=decrement, bouncetime=300) 

# Logic that you write
def main():
#    print("write your logic here")
        print("Push button to toggle LED")
	print(index)
        time.sleep(1)

# Only run the functions if
if __name__ == "__main__":
    # Make sure the GPIO is stopped correctly
    try:
        while True:
            main()
    except KeyboardInterrupt:
	print("Exiting gracefully")
        # Turn off your GPIOs here
        GPIO.cleanup()
    except e:
        GPIO.cleanup()
        print("Some other error occurred")
        print(e.message)
 
