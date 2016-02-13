// TimeDisplay.c
// TM4C123


#include <stdint.h>
#include <stdio.h>
#include "../Shared/tm4c123gh6pm.h"
#include "ST7735.h"
#include <math.h>
#include "TimeDisplay.h"


#define CENTER_X 63
#define CENTER_Y 79
#define PI 3.14159265

extern long StartCritical (void);    // previous I bit, disable interrupts
extern void EndCritical(long sr);    // restore I bit to previous value

extern volatile uint16_t Time_Minutes, Time_Hours; 
volatile uint32_t Display_Mode;



//------------TimeDisplay_Init------------
// Initialize GPIO Port A bit 5 for input
// Input: none
// Output: none
void TimeDisplay_Init(void){ 
	Display_Mode=0;
	DisplayRefresh();
}
void DisplayMinute(void){
	ST7735_Line(CENTER_X,CENTER_Y,CENTER_X+60*sin(Time_Minutes*6*PI/180),CENTER_Y+60*cos(Time_Minutes*6*PI/180),ST7735_BLACK);
}
void DisplayHour(void){
	ST7735_Line(CENTER_X,CENTER_Y,CENTER_X+30*sin(Time_Hours*6*PI/180),CENTER_Y+30*cos(Time_Hours*6*PI/180),ST7735_BLACK);
}
void DisplayDigital(void){
	char ch[5];
	sprintf(ch,"%.2d:%.2d",Time_Hours, Time_Minutes);
	ST7735_SetCursor(50, CENTER_Y);
	ST7735_OutString(ch);
}
void DisplayRefresh(void){
	if(Display_Mode==0){
		DisplayMinute();
		DisplayHour();
	}
	else
		DisplayDigital();
}
void SwitchMode(void){
	long critical= StartCritical();
		if(Display_Mode==0)
			Display_Mode =1;
		else
			Display_Mode=0;
		EndCritical(critical);
}


