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
volatile uint16_t StopWatch_Seconds, StopWatch_Minutes, StopWatch_Hours; //from Main Class
volatile uint16_t Stopwatch_Triggered;

void StopWatch_Init(void)
{
	Stopwatch_Triggered=0;
}
void DisplayStopwatch(void)
{
	char AsciiArray[] = {'0','1','2','3','4','5','6','7','8','9'};
	char ch[8];
	CheckStopwatchTime();
	ch[0] = AsciiArray[Time_Hours/10];
	ch[1] = AsciiArray[Time_Hours%10];
	ch[2] = ':';
	ch[3] = AsciiArray[Time_Minutes/10];
	ch[4] = AsciiArray[Time_Minutes%10];
	
	Output_Clear();
	ST7735_DrawCharS(0, 50, ch[0], ST7735_YELLOW, ST7735_BLACK, 4);
	ST7735_DrawCharS(25, 50, ch[1], ST7735_YELLOW, ST7735_BLACK, 4);
	ST7735_DrawCharS(50, 50, ch[2], ST7735_YELLOW, ST7735_BLACK, 4);
	ST7735_DrawCharS(75, 50, ch[3], ST7735_YELLOW, ST7735_BLACK, 4);
	ST7735_DrawCharS(100, 50, ch[4], ST7735_YELLOW, ST7735_BLACK, 4);
}

void CheckStopwatchTime(void){
	if(Stopwatch_Triggered=0){
	}
	else{
		
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

