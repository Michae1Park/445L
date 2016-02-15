// TimeDisplay.c
// TM4C123


#include <stdint.h>
#include <stdio.h>
#include "../Shared/tm4c123gh6pm.h"
#include "ST7735.h"
#include <math.h>
#include "TimeDisplay.h"
#include "Timer1.h"



#define CENTER_X 64
#define CENTER_Y 80
#define LENGTH_S 45.0
#define LENGTH_M 45.0
#define LENGTH_H 45.0
#define PI 3.14159265358979323846

 
extern long StartCritical (void);    // previous I bit, disable interrupts
extern void EndCritical(long sr);    // restore I bit to previous value
void DelayWait10ms(uint32_t n);
void EraseSecond(void);

//extern volatile uint16_t Time_Minutes, Time_Hours; 
volatile uint32_t Display_Mode;
volatile double x, y;
volatile static double prevsx, prevsy, prevmx, prevmy, prevhx, prevhy;

//------------TimeDisplay_Init------------
// Initialize GPIO Port A bit 5 for input
// Input: none
// Output: none
void TimeDisplay_Init(void){ 
	Display_Mode=0;
	DisplayRefresh();
}


void DisplaySecond(void)
{
	x = 0;
	y = 0;
	x = CENTER_X + LENGTH_S*cos(Time_Seconds*6*(PI/180));
	y = CENTER_Y + LENGTH_S*sin(Time_Seconds*6*(PI/180));
	prevsx = x;
	prevsy = y;
	
	ST7735_Line(CENTER_X, CENTER_Y, x, y, ST7735_WHITE);
}

void EraseSecond(void)
{
	if (((prevsx==prevmx)&&(prevsy==prevmy)) || ((prevsx==prevmx)&&(prevsy==prevmy))){}
	else{ ST7735_Line(CENTER_X, CENTER_Y, prevsx, prevsy, ST7735_BLACK);}
}
void DisplayMinute(void)
{
	x = 0;
	y = 0;
	x = CENTER_X + LENGTH_M*cos(Time_Minutes*6*(PI/180));
	y = CENTER_Y + LENGTH_M*sin(Time_Minutes*6*(PI/180));
	prevmx = x;
	prevmy = y;
	
	ST7735_Line(CENTER_X, CENTER_Y, x, y, ST7735_WHITE);
}

void EraseMinute(void)
{
	if (((prevmx==prevhx)&&(prevmy==prevhy))){}
	else{ ST7735_Line(CENTER_X, CENTER_Y, prevmx, prevmy, ST7735_BLACK);}
}

void DisplayHour(void)
{
	x = 0;
	y = 0;
	x = CENTER_X + LENGTH_H*cos(Time_Hours*6*(PI/180));
	y = CENTER_Y + LENGTH_H*sin(Time_Hours*6*(PI/180));
	prevhx = x;
	prevhy = y;
	
	ST7735_Line(CENTER_X, CENTER_Y, x, y, ST7735_WHITE);
}

void EraseHour(void)
{
	ST7735_Line(CENTER_X, CENTER_Y, prevhx, prevhy, ST7735_BLACK);
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

void ClockFace_Init(void)
{
    uint32_t r = 60;
    ST7735_DrawCircle(64, 80, r, ST7735_Color565(0, 0, 255));  // clock face
		ST7735_DrawCharS(57, 22, '1', ST7735_Color565(255, 0, 0), 0, 1);
    ST7735_DrawCharS(62, 22, '2', ST7735_Color565(255, 0, 0), 0, 1);
		ST7735_DrawCharS(117, 77, '3', ST7735_Color565(255, 128, 0), 0, 1);
    ST7735_DrawCharS(61, 132, '6', ST7735_Color565(255, 0, 0), 0, 1);
    ST7735_DrawCharS(6, 77, '9', ST7735_Color565(255, 0, 0), 0, 1);
    DelayWait10ms(1);     
}

// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n)
{
	uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}
