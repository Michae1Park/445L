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

extern long StartCritical (void);    // previous I bit, disable interrupts
extern void EndCritical(long sr);    // restore I bit to previous value

void changeTime(void);
void WaitForInterrupt(void);  // low power mode


void changeTime(void)
{
	active_In10s = 1;
	
	TIMER1_CTL_R = 0x00000000;    //disable TIMER1A 
	Output_Clear();
  ClockFace_Init();
	DisplayHour();
	DisplayMinute();
	DisplaySecond();
  
	while (active_In10s)
	{
		switch(Mode) 
		{
			case 0:
				Mode = 0xFFFF;
        incrementHour();
				EraseHour();
				DelayWait10ms(2);
				DisplayHour();
        DelayWait10ms(2);
				break;
      case 1:
				Mode = 0xFFFF;
        decrementHour();
				EraseHour();
				DelayWait10ms(2);
				DisplayHour();	
        DelayWait10ms(2);
				break;
      case 2:
				Mode = 0xFFFF;
        incrementMin();
				EraseMinute();
				DelayWait10ms(2);
				DisplayMinute();
				DelayWait10ms(2);
        break;
      case 3:
				Mode = 0xFFFF;
        decrementMin();
				EraseMinute();
				DelayWait10ms(2);
				DisplayMinute();
				DelayWait10ms(2);
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

