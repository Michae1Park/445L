// SetTime.c
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for functions to set time
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/15/16

#include <stdio.h>
#include <stdint.h>
#include "SetTime.h"
#include "Timer1.h"
#include "TimeDisplay.h"
#include "../Shared/tm4c123gh6pm.h"
#include "Common.h"
#include "ST7735.h"

void changeTime(void)
{
	TIMER1_CTL_R = 0x00000000;    //disable TIMER1A 
	Output_Clear();
	DisplaySetTime();
  
	while (active_In10s)
	{
		switch(Mode) 
		{
			case 0:
				Mode = 0xFFFF;
        incrementHour();
				DisplaySetTime();
				DelayWait10ms(1);
				break;
      case 1:
				Mode = 0xFFFF;
        decrementHour();
				DisplaySetTime();	
        DelayWait10ms(1);
				break;
      case 2:
				Mode = 0xFFFF;
				incrementMin();
				DisplaySetTime();
				DelayWait10ms(1);
        break;
      case 3:
				Mode = 0xFFFF;
				decrementMin();
				DisplaySetTime();
				DelayWait10ms(1);
        break;
			default:
				Mode = 0xFFFF;
				break;
		}
	}	
	TIMER1_CTL_R = 0x00000001;    //enable TIMER1A 
}

void incrementHour(void)
{
  if((Time_Hours+1)>23) {Time_Hours = 0;} //if hour exceeds 23, reset to 0
  else {Time_Hours++;}
}

void decrementHour(void)
{
  if((Time_Hours-1)<0) {Time_Hours = 23;} //if hour below 0, reset to 23
  else {Time_Hours--;}
}

void incrementMin(void)
{
  if((Time_Minutes+1)>59) {Time_Minutes = 0;} //if min exceeds 59, reset to 0
  else {Time_Minutes++;}
}

void decrementMin(void)
{
  if((Time_Minutes-1)<0) {Time_Minutes = 59;} //if min below 0, reset to 59
  else {Time_Minutes--;}
}

