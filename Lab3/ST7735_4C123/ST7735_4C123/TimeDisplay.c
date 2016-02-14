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
#define PI 3.14159265358979323846
 
extern long StartCritical (void);    // previous I bit, disable interrupts
extern void EndCritical(long sr);    // restore I bit to previous value
void DelayWait10ms(uint32_t n);

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
	ST7735_Line(CENTER_X,CENTER_Y,CENTER_X+60*sin(Time_Minutes*6*PI/180),CENTER_Y+60*cos(Time_Minutes*6*PI/180),ST7735_YELLOW);
}
void DisplayHour(void){
	ST7735_Line(CENTER_X,CENTER_Y,CENTER_X+30*sin(Time_Hours*6*PI/180),CENTER_Y+30*cos(Time_Hours*6*PI/180),ST7735_YELLOW);
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


void drawClockHands(void){
    double lineLength = 45.0;
    double rX = 64;
    double rY = 80;
    double x2 = 0;
    double y2 = 0;
    int sec = 0;
    for(int angle = 0; angle < 360; angle+=6){
        x2 = rX + lineLength*cos(angle * (PI/180));
        y2 = rY + lineLength*sin(angle * (PI/180));
        if(sec%10 == 0){
            printf("%d", sec);
        }
        sec++;
        ST7735_Line(rX, rY, x2, y2, ST7735_GREEN);
        DelayWait10ms(500);
        ST7735_Line(rX, rY, x2, y2, ST7735_YELLOW);
			}  
}

void ClockFace_Init(void)
{
    int radius = 60;
    ST7735_DrawCircle(64, 80, radius, ST7735_Color565(0, 0, 255));  // clock face
		ST7735_DrawCharS(57, 22, '1', ST7735_Color565(255, 0, 0), 0, 1);
    ST7735_DrawCharS(62, 22, '2', ST7735_Color565(255, 0, 0), 0, 1);
		ST7735_DrawCharS(117, 77, '3', ST7735_Color565(255, 128, 0), 0, 1);
    ST7735_DrawCharS(61, 132, '6', ST7735_Color565(255, 0, 0), 0, 1);
    ST7735_DrawCharS(6, 77, '9', ST7735_Color565(255, 0, 0), 0, 1);
    DelayWait10ms(1);
    //drawClockHands();
     
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
