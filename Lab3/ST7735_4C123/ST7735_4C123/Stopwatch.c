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
#include "ST7735.h"
#include "Stopwatch.h"
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
	
	ch[0] = AsciiArray[StopWatch_Hours/10];
	ch[1] = AsciiArray[StopWatch_Hours%10];
	ch[2] = ':';
	ch[3] = AsciiArray[StopWatch_Minutes/10];
	ch[4] = AsciiArray[StopWatch_Minutes%10];
	ch[5] = ':';
	ch[6] = AsciiArray[StopWatch_Seconds/10];
	ch[7] = AsciiArray[StopWatch_Seconds%10];
	
	Output_Clear();
	ST7735_DrawCharS(0, 50, ch[0], ST7735_YELLOW, ST7735_BLACK, 3);
	ST7735_DrawCharS(15, 50, ch[1], ST7735_YELLOW, ST7735_BLACK, 3);
	ST7735_DrawCharS(30, 50, ch[2], ST7735_YELLOW, ST7735_BLACK, 3);
	ST7735_DrawCharS(45, 50, ch[3], ST7735_YELLOW, ST7735_BLACK, 3);
	ST7735_DrawCharS(60, 50, ch[4], ST7735_YELLOW, ST7735_BLACK, 3);
	ST7735_DrawCharS(75, 50, ch[5], ST7735_YELLOW, ST7735_BLACK, 3);
	ST7735_DrawCharS(90, 50, ch[6], ST7735_YELLOW, ST7735_BLACK, 3);
	ST7735_DrawCharS(105, 50, ch[7], ST7735_YELLOW, ST7735_BLACK, 3);
	
	
	//StopWatch_Seconds=0;
	//StopWatch_Minutes=0;
	//StopWatch_Hours=0;
	
	

}

void triggerStopWatch(void){
	StopWatch_Seconds=Time_Seconds;
	StopWatch_Minutes=Time_Minutes;
	StopWatch_Hours=Time_Hours;
}
void endStopWatch(void){
	StopWatch_Seconds=Time_Seconds-StopWatch_Seconds;
	StopWatch_Minutes=Time_Minutes-StopWatch_Minutes;
	StopWatch_Hours=Time_Hours-StopWatch_Hours;
}


