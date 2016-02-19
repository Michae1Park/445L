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
#include "ToggleAlarm.h"
#include "../Shared/tm4c123gh6pm.h"


extern long StartCritical (void);    // previous I bit, disable interrupts
extern void EndCritical(long sr);    // restore I bit to previous value
extern volatile uint16_t Time_Seconds, Time_Minutes, Time_Hours; //from Main Class
extern volatile uint16_t Alarm_Seconds, Alarm_Minutes, Alarm_Hours; //from Main Class
volatile uint16_t Alarm_Triggered;

void ToggleAlarm_Init(void)
{
	Alarm_Triggered=0;
}
void CheckAlarmTrigger(void){
	//disable interrupts
	if(Alarm_Triggered==0){
		CheckAlarmTime();
	}
	else{
		//perform sound
	}
}

void ToggleAlarmTrigger(void){
	long critical= StartCritical();
		if(Alarm_Triggered==0)
			Alarm_Triggered =1;
		else
			Alarm_Triggered=0;
		EndCritical(critical);
}
//Assuming Time interrupt is disalbed
void CheckAlarmTime(void)
{
	if(Time_Minutes==Alarm_Minutes && Time_Hours==Alarm_Hours)
	{
		ToggleAlarmTrigger();
	}
	
}

