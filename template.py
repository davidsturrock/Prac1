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
import time
# Setup
GPIO.setmode(GPIO.BCM)
GPIO.setup(17, GPIO.OUT)
GPIO.output(17,GPIO.LOW)
# Logic that you write
def main():
#    print("write your logic here")
	GPIO.output(17,GPIO.HIGH)
	time.sleep(1)
	GPIO.output(17,GPIO.LOW)
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
