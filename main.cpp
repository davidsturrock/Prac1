/* Includes */
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#include <softPwm.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <iostream>
#include <string> // for string class
#include <vector>
#include <unistd.h>
#include <pthread.h>

using namespace std;

/****************************Global variables*****************************/
int hh,mm,ss;
int HH,MM,SS;
int x=1000, count=0;
string Alarm;
int hours, mins, secs;
string ch0,ch1,ch2;
long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance
bool start = true, startCount = false;
float DAC;
bool alarmT = false;
const char RTCAddr = 0x6f;
const char SEC = 0x00; // see register table in datasheet
const char MIN = 0x01;
const char HOUR = 0x02;
const char TIMEZONE = 0; // +02H00 (RSA)

const int BTNS[] = {0,2,3,7};

/*********************************Interrupts*****************************/
void stop_start_monitoring(){
	if ((interruptTime-lastInterruptTime)>200){
		start = !start;
	}
	lastInterruptTime = interruptTime;
}

void dismiss_alarm(){
	if ((interruptTime-lastInterruptTime)>200){
		Alarm = "";
		alarmT = false;
		startCount = true;
	}
	lastInterruptTime = interruptTime;
}

void reset_system_time(){
	if ((interruptTime-lastInterruptTime)>200){
		System("clear");
		wiringPiI2CWriteReg8(RTC, HOUR, 0x0+TIMEZONE);
		wiringPiI2CWriteReg8(RTC, MIN, 0x0);
		wiringPiI2CWriteReg8(RTC, SEC, 0x80);
	}
	lastInterruptTime = interruptTime;
}

void change_reading_interval(){
	if ((interruptTime-lastInterruptTime)>200){
		if (x==1000){
			x=2000;
		}
		else if (x==2000){
			x=5000;
		}
		else {x=1000;}
	}
	lastInterruptTime = interruptTime;
}

/***********************Functions***********************************/
void init_buttons_and_RTC(){
	wiringPiSetup(); // Set up wiringPi

	RTC = wiringPiI2CSetup(RTCAddr); // Set up I2C

	// Set up buttons
	for(int i=0; i < 3; i++){
			pinMode(BTNS[i], INPUT);
			pullUpDnControl(BTNS[i], PUD_UP);
	}
	wiringPiISR(BTNS[0],INT_EDGE_RISING, &stop_start_monitoring);
	wiringPiISR(BTNS[1],INT_EDGE_RISING, &dismiss_alarm);
	wiringPiISR(BTNS[2],INT_EDGE_RISING, &reset_system_time);
	wiringPiISR(BTNS[3],INT_EDGE_RISING, &change_reading_interval);
}

/******************TIME CODE************************/

void getCurrentTime(void){
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  HH = timeinfo ->tm_hour;
  MM = timeinfo ->tm_min;
  SS = timeinfo ->tm_sec;
}

int getHours(void){
    getCurrentTime();
    return HH;
}

int getMins(void){
    return MM;
}

int getSecs(void){
    return SS;
}

int hFormat(int hours){
	if (hours >= 24){
		hours = 0;
	}
	else if (hours > 12){
		hours -= 12;
	}
	return (int)hours;
}

int hexCompensation(int units){

	int unitsU = units%0x10;

	if (units >= 0x50){
		units = 50 + unitsU;
	}
	else if (units >= 0x40){
		units = 40 + unitsU;
	}
	else if (units >= 0x30){
		units = 30 + unitsU;
	}
	else if (units >= 0x20){
		units = 20 + unitsU;
	}
	else if (units >= 0x10){
		units = 10 + unitsU;
	}
	return units;
}


/***********************SENSOR CODE*************************************/

// read a channel
int mcp3008_read(uint8_t adcnum)
{
	unsigned int commandout = 0;
	unsigned int adcout = 0;

	commandout = adcnum & 0x3;  // only 0-7
	commandout |= 0x18;     // start bit + single-ended bit

	uint8_t spibuf[3];

	spibuf[0] = commandout;
	spibuf[1] = 0;
	spibuf[2] = 0;

	wiringPiSPIDataRW(1, spibuf, 3);

	adcout = ((spibuf[1] << 8) | (spibuf[2])) >> 4;

	//printf("%d\n", adcout);
	return adcout;

}

/*
void *playThread(void *threadargs){
    // If the thread isn't ready, don't do anything
    while( threadReady)
        continue;

    pthread_exit(NULL);
}*/


/********************* Main ************************************/
int main(void){
	init_buttons_and_RTC();
	wiringPiSetupGpio();

	wiringPiI2CWriteReg8(RTC, HOUR, 0x0+TIMEZONE);
	wiringPiI2CWriteReg8(RTC, MIN, 0x0);
	wiringPiI2CWriteReg8(RTC, SEC, 0x80);

	wiringPiSPISetup(1, 4*1000*1000); // Change this later

	// Repeat this until we shut down
	for (;;){
		// System time
		hours = hexCompensation(wiringPiI2CReadReg8(RTC, HOUR));
		mins = hexCompensation(wiringPiI2CReadReg8(RTC, MIN));
		secs = hexCompensation(wiringPiI2CReadReg8(RTC, SEC)-0x80);

		// RTC time
		HH = getHours();
		MM = getMins();
		SS = getSecs();

		/*************** Try this later (for threads)

		pthread_attr_t tattr;
		pthread_t thread_id;
		int newprio = 99;
		sched_param param;

		pthread_attr_init (&tattr);
		pthread_attr_getschedparam (&tattr, &param); /* safe to get existing scheduling param
		param.sched_priority = newprio; /* set the priority; others are unchanged
		pthread_attr_setschedparam (&tattr, &param); /* setting the new scheduling param
		pthread_create(&thread_id, &tattr, playThread, (void *)1); /* with new priority specified

		if(!threadReady){
			threadReady = true;
		}

		printf(ch0);
		printf(ch1);
		printf(ch2);

		//Join and exit the playthread
		pthread_join(thread_id, NULL);
		pthread_exit(NULL);

		*/

		// Sensor values
		ch0 = to_string(mcp3008_read(0)); // Temperature
		ch1 = to_string(mcp3008_read(1)); // Humidity
		ch2 = to_string(mcp3008_read(2)); // Light
		DAC = (ch2/1023)*ch1;

		// Check Alarm status
		if (startCount == true){
			count += x;
			if (count>180){
				count = 0;
				startCount = false;
			}

		if (DAC<0.65){
			if(alarmT==false && startCount == false){
				alarmT = true;
			}
		}
		else if (DAC>2.65 && startCount==false){
				alarmT = true;
			}
		}

		if(alarmT == true){
			Alarm = " *";
		}
		else{ Alarm = "";
		}

		string System_time = to_string(hours)+":"+to_string(mins)+":"+to_string(secs);
		string RTC_time = to_string(HH)+":"+to_string(MM)+":"+to_string(SS);

		// Print out the values
		printf("-------------------------------------------------------------------------------------------------\n");
		std:: cout<<"| RTC Time |"<<" Sys Time |"<<" Humidity "<<"| Temp |"<<" Light |"<<" DAC out |"<<" Alarm |\n";

		if(start==true){
			std:: cout<<"| "<< RTC_time<<" | "<<System_time<<" | "<<ch1<< " | "<<ch0<<" | "<<ch2<<" | "<<DAC<<" | "<<Alarm<<" |\n";
		}

		// Variable x used to change monitoring frequency
		delay(x); //milliseconds
	}
		return 0;
}
