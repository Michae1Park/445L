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

void SetTime_Init(void)
{

}
void DisplaySetTime(void){
	//disable interrupts
	long critical=StartCritical();
	//Display_Mode=1;
	DisplayRefresh();
	EndCritical(critical);
}

void changeTime(void)
{
	TIMER1_CTL_R = 0x00000000;    //disable TIMER1A 
	Output_Clear();
  ClockFace_Init();
	DisplayHour();
	DisplayMinute();
	DisplaySecond();
   

	switch(Mode) 
  {
      case 0:
        incrementHour();
				EraseHour();
				DisplayHour();
        break;
      case 1:
        decrementHour();
				EraseHour();
				DisplayHour();	
        break;
      case 2:
        incrementMin();
				EraseMinute();
				DisplayMinute();
        break;
      case 3:
        decrementMin();
				EraseMinute();
				DisplayMinute();
        break;
			default:
				break;
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

