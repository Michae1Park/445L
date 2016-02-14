// SetTime.c
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for functions to set time
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/15/16

#include <stdio.h>
#include <stdint.h>
#include "SetAlarm.h"
#include "ST7735.h"
#include "../Shared/tm4c123gh6pm.h"

extern long StartCritical (void);    // previous I bit, disable interrupts
extern void EndCritical(long sr);    // restore I bit to previous value
extern volatile uint16_t Time_Seconds, Time_Minutes, Time_Hours; //from Main Class
volatile uint16_t Alarm_Seconds, Alarm_Minutes, Alarm_Hours; //from Main Class

void SetAlarm_Init(void)
{
	Alarm_Seconds=0;
	Alarm_Minutes=0;
	Alarm_Hours=0;
}
void DisplayAlarmTime(void){
	//disable interrupts
	long critical=StartCritical();
	DisplayAlarm();
	EndCritical(critical);
}
//Assuming Time interrupt is disalbed
void changeAlarmTime(uint32_t buttonSelect)
{
		//enable critical section
		long critical=StartCritical();
	
		//set time seconds to 0
		Alarm_Seconds=0;
	
    switch(buttonSelect) 
    {
      case 0:
        incrementAlarmHour();
        break;
      case 1:
        decrementAlarmHour();
        break;
      case 2:
        incrementAlarmMin();
        break;
      case 3:
        decrementAlarmMin();
        break;
		}
		EndCritical(critical);
}

void incrementAlarmHour(void)
{
  if((++Alarm_Hours)>23) {Alarm_Hours = 0;} //if hour exceeds 23, reset to 0
  else {Alarm_Hours++;}
}

void decrementAlarmHour(void)
{
  if((--Alarm_Hours)<0) {Alarm_Hours = 23;} //if hour below 0, reset to 23
  else {Alarm_Hours--;}
}

void incrementAlarmMin(void)
{
  if((++Alarm_Minutes)>59) {Alarm_Minutes = 0;} //if min exceeds 59, reset to 0
  else {Alarm_Minutes++;}
}

void decrementAlarmMin(void)
{
  if((--Alarm_Minutes)<0) {Alarm_Minutes = 59;} //if min below 0, reset to 59
  else {Alarm_Minutes--;}
}
void DisplayAlarm(void){
	char ch[5];
	sprintf(ch,"%.2d:%.2d",Alarm_Hours, Alarm_Minutes);
	ST7735_SetCursor(50, 79);
	ST7735_OutString(ch);
}
