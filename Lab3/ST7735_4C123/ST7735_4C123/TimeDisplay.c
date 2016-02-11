// TimeDisplay.c
// TM4C123


#include <stdint.h>
#include <stdio.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include <math.h>
#include "TimeDisplay.h"
#define CENTER_X 63
#define CENTER_Y 79
#define PI 3.14159265


uint32_t getMinutes(uint32_t time);
uint32_t getHours(uint32_t time);

volatile uint32_t DisplayMode=0;

//------------Switch_Init------------
// Initialize GPIO Port A bit 5 for input
// Input: none
// Output: none
void TimeDisplay_Init(void){ 
	DisplayRefresh(hours, minutes);
}
void DisplayMinute(uint32_t minutes){
	uint32_t min=minutes;
	ST7735_Line(CENTER_X,CENTER_Y,CENTER_X+60*sin(min*6*PI/180),CENTER_Y+60*cos(min*6*PI/180),ST7735_BLACK);
}
void DisplayHour(uint32_t hours){
	uint32_t hour=hours;
	ST7735_Line(CENTER_X,CENTER_Y,CENTER_X+30*sin(hour*6*PI/180),CENTER_Y+30*cos(hour*6*PI/180),ST7735_BLACK);
}
void DisplayDigital(uint32_t hours, uint32_t minutes){
	uint32_t hour=hours;
	uint32_t min=minutes;
	char ch[5];
	sprintf(ch,"%.2d:%d",hour, min);
	ST7735_setCursor(50,79);
	ST7732_outString(ch);

}
void DisplayRefresh(uint32_t hours, uint32_t minutes){
	if(displayflag){
		if(DisplayMode==0){
			DisplayMinute(minutes);
			DisplayHour(hours);
		}
		else
			DisplayDigital(hours,minutes);
	}
}
void SwitchModes(void){
	if(switchflag){
		if(DisplayMode==0)
			DisplayMode =1;
		else
			DisplayMode=0;
	}
}


