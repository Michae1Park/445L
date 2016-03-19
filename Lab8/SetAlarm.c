// SetTime.c
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for functions to set alarm
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/15/16

#include <stdio.h>
#include <stdint.h>
#include "SetTime.h"
#include "Timer1.h"
#include "Display.h"
#include "../Shared/tm4c123gh6pm.h"
#include "Common.h"
#include "ST7735.h"
#include "SetAlarm.h"

volatile uint16_t alarm_hours=0;
volatile uint16_t alarm_minutes=0;
volatile uint16_t alarm_flag = 1;


//Assuming Time interrupt is disalbed
void setAlarmTimeBase(void)
{
	Output_Clear();
	DisplayAlarm();
  
	while (active_In10s)
	{
		switch(Mode) 
		{
			case 0:
				Mode = 0xFFFF;
        incrementAlarmHour();
				DisplayAlarm();
				DelayWait10ms(1);
				break;
      case 1:
				Mode = 0xFFFF;
        decrementAlarmHour();
				DisplayAlarm();	
        DelayWait10ms(1);
				break;
      case 2:
				Mode = 0xFFFF;
				incrementAlarmMin();
				DisplayAlarm();
				DelayWait10ms(1);
        break;
      case 3:
				Mode = 0xFFFF;
				decrementAlarmMin();
				DisplayAlarm();
				DelayWait10ms(1);
        break;
			default:
				Mode = 0xFFFF;
				break;
		}
	}	

}

void incrementAlarmHour(void)
{
  if((alarm_hours+1)>23) {alarm_hours = 0;} //if hour exceeds 23, reset to 0
  else {alarm_hours++;}
}

void decrementAlarmHour(void)
{
  if((alarm_hours-1)<0) {alarm_hours = 23;} //if hour below 0, reset to 23
  else {alarm_hours--;}
}

void incrementAlarmMin(void)
{
  if((alarm_minutes+1)>59) {alarm_minutes = 0;} //if min exceeds 59, reset to 0
  else {alarm_minutes++;}
}

void decrementAlarmMin(void)
{
  if((alarm_minutes-1)<0) {alarm_minutes = 59;} //if min below 0, reset to 59
  else {alarm_minutes--;}
}
void AlarmToggleON(void){

		if(alarm_flag==0)
			alarm_flag =1;
	
}
void AlarmToggleOFF(void){
			if(alarm_flag==1)
			alarm_flag =0;
	
}