// ATimeDisplay.c
// Michael Park, Jack Zhao
// Date Created: 2/12/2016
// Includes analog and digital time display functions
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 2/15/2016

#include <stdint.h>
#include <stdio.h>
#include "../Shared/tm4c123gh6pm.h"
#include "ST7735.h"
#include <math.h>
#include "Display.h"
#include "Timer1.h"
#include "SetAlarm.h"
#include "Common.h"
#include "Systick.h"
#include "Stopwatch.h"

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
volatile uint16_t display_mode = 0;
//volatile double x, y;
volatile static double prevsx, prevsy, prevmx, prevmy, prevhx, prevhy;
extern volatile uint16_t Stopwatch_Triggered;
extern volatile uint16_t alarm_minutes, alarm_hours;
extern volatile uint16_t Time_minutes, Time_hours;
extern volatile uint16_t alarm_flag;
extern char weather_temp[3];
extern char weather_weather[6];

void DisplayAlarm(void)
{
	char AsciiArray[] = {'0','1','2','3','4','5','6','7','8','9'};
	char ch[5];
	ch[0] = AsciiArray[alarm_hours/10];
	ch[1] = AsciiArray[alarm_hours%10];
	ch[2] = ':';
	ch[3] = AsciiArray[alarm_minutes/10];
	ch[4] = AsciiArray[alarm_minutes%10];
	
	Output_Clear();
	ST7735_DrawCharS(0, 50, ch[0], ST7735_YELLOW, ST7735_BLACK, 4);
	ST7735_DrawCharS(25, 50, ch[1], ST7735_YELLOW, ST7735_BLACK, 4);
	ST7735_DrawCharS(50, 50, ch[2], ST7735_YELLOW, ST7735_BLACK, 4);
	ST7735_DrawCharS(75, 50, ch[3], ST7735_YELLOW, ST7735_BLACK, 4);
	ST7735_DrawCharS(100, 50, ch[4], ST7735_YELLOW, ST7735_BLACK, 4);
}

void DisplaySetTime(void)
{
	char AsciiArray[] = {'0','1','2','3','4','5','6','7','8','9'};
	char ch[5];
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


//Display
//Builtint Clock
//Builtin Alarm time
//Pulled Weather temperature
//							 Rainy, Cloudy, Sunny, etc
//
//Recommended Outfit
//						100+ : Topless
//						80+	 : Shorts 		 + 	Short Sleeves
//						75+	 : Shorts 		 + 	Long Sleeves
//						60+	 : Pants/Jeans + 	Short Sleeves (optional jacket)
//						50+	 : Pants/Jeans +	Long Sleeves
//						35+	 : Pants/Jeans +  Long Sleeves + Jacket/Hoodie
//						35-  : Coat
//SW1	: SetTime
//SW2 : SetAlarm
//SW3	:	Snooze
//SW4	: Update Weather Data, Recommneded Outfit will follow
//System Automatically updates at Midnight, Else update switch needs to be pressed to update
void Display_PG1(void)
{
	char AsciiArray[] = {'0','1','2','3','4','5','6','7','8','9'};
	char ch[8];
	int weather_temp_int = 88;

//TODO: CONVERT DATA PULLED (STR) into INTEGER!	
//test code
//	weather_temp[0] = '8';
//	weather_temp[1] = '8';
//test code
//	weather_temp_int = Convert_StringToInteger(weather_temp);
	
	Output_Clear();
	
	//Time Display
	ST7735_SetCursor(0, 0);
	
	ch[0] = AsciiArray[Time_Hours/10];
	ch[1] = AsciiArray[Time_Hours%10];
	ch[2] = ':';
	ch[3] = AsciiArray[Time_Minutes/10];
	ch[4] = AsciiArray[Time_Minutes%10];
	ch[5]	= ':';
	ch[6] = AsciiArray[Time_Seconds/10];
	ch[7] = AsciiArray[Time_Seconds%10];

	ST7735_DrawCharS(0, 0, ch[0], ST7735_YELLOW, ST7735_BLACK, 2);
	ST7735_DrawCharS(15, 0, ch[1], ST7735_YELLOW, ST7735_BLACK, 2);
	ST7735_DrawCharS(30, 0, ch[2], ST7735_YELLOW, ST7735_BLACK, 2);
	ST7735_DrawCharS(45, 0, ch[3], ST7735_YELLOW, ST7735_BLACK, 2);
	ST7735_DrawCharS(60, 0, ch[4], ST7735_YELLOW, ST7735_BLACK, 2);
//	ST7735_DrawCharS(75, 0, ch[5], ST7735_YELLOW, ST7735_BLACK, 2);
//	ST7735_DrawCharS(90, 0, ch[6], ST7735_YELLOW, ST7735_BLACK, 2);
//	ST7735_DrawCharS(105, 0, ch[7], ST7735_YELLOW, ST7735_BLACK, 2);
		
	if(displayFlag == 0x03) 
	{
		displayFlag = 0xFF;
		Output_Clear();
		ST7735_DrawCharS(0, 0, ch[0], ST7735_YELLOW, ST7735_BLACK, 2);
		ST7735_DrawCharS(15, 0, ch[1], ST7735_YELLOW, ST7735_BLACK, 2);
		ST7735_DrawCharS(30, 0, ch[2], ST7735_YELLOW, ST7735_BLACK, 2);
		ST7735_DrawCharS(45, 0, ch[3], ST7735_YELLOW, ST7735_BLACK, 2);
		ST7735_DrawCharS(60, 0, ch[4], ST7735_YELLOW, ST7735_BLACK, 2);
//		ST7735_DrawCharS(75, 0, ch[5], ST7735_YELLOW, ST7735_BLACK, 2);
//		ST7735_DrawCharS(90, 0, ch[6], ST7735_YELLOW, ST7735_BLACK, 2);
//		ST7735_DrawCharS(105, 0, ch[7], ST7735_YELLOW, ST7735_BLACK, 2);
	}
	if(displayFlag == 0x07) 
	{
		displayFlag = 0xFF;
		Output_Clear();
		ST7735_DrawCharS(0, 0, ch[0], ST7735_YELLOW, ST7735_BLACK, 2);
		ST7735_DrawCharS(15, 0, ch[1], ST7735_YELLOW, ST7735_BLACK, 2);
		ST7735_DrawCharS(30, 0, ch[2], ST7735_YELLOW, ST7735_BLACK, 2);
		ST7735_DrawCharS(45, 0, ch[3], ST7735_YELLOW, ST7735_BLACK, 2);
		ST7735_DrawCharS(60, 0, ch[4], ST7735_YELLOW, ST7735_BLACK, 2);
//		ST7735_DrawCharS(75, 0, ch[5], ST7735_YELLOW, ST7735_BLACK, 2);
//		ST7735_DrawCharS(90, 0, ch[6], ST7735_YELLOW, ST7735_BLACK, 2);
//		ST7735_DrawCharS(105, 0, ch[7], ST7735_YELLOW, ST7735_BLACK, 2);	
	}
	
	//Alarm Display
	ST7735_DrawString(0, 2, "ALARM", ST7735_GREEN);
	ST7735_SetCursor(7, 2);
	ST7735_OutUDec(alarm_hours);
	ST7735_OutString(":");
	ST7735_OutUDec(alarm_minutes);
	
	//Weather
	ST7735_DrawString(0, 5, "WEATHER", ST7735_GREEN);
	ST7735_DrawString(0, 6, "Austin: ", ST7735_GREEN);	//Substitute Temperature data with data pulled from server
	ST7735_DrawString(8, 6, "77F", ST7735_GREEN);	//Substitute Temperature data with data pulled from server
	ST7735_DrawString(8, 7, "Rainy", ST7735_GREEN);				//Substitute Weather data with data pulled from server
																												//Maybe insert an icon image to show weather
	
//Recommendation
//Recommended Outfit
//						100+ : Topless
//						80+	 : Shorts 		 + 	Short Sleeves
//						75+	 : Shorts 		 + 	Long Sleeves
//						60+	 : Pants/Jeans + 	Short Sleeves (optional jacket)
//						50+	 : Pants/Jeans +	Long Sleeves
//						35+	 : Pants/Jeans +  Long Sleeves + Jacket/Hoodie
//						35-  : Coat
	ST7735_DrawString(0, 10, "SUGGESTED OUTFIT", ST7735_CYAN);
	if(weather_temp_int >= 100) { ST7735_DrawString(4, 11, "TOPLESS", ST7735_CYAN);}
	else if((weather_temp_int >= 80) && (weather_temp_int < 100)) 
	{ 
		ST7735_DrawString(3, 11, "SHORTS & ", ST7735_CYAN);
		ST7735_DrawString(3, 12, "SHORT SLEEVES", ST7735_CYAN);
	}
	else if((weather_temp_int >= 75) && (weather_temp_int < 80)) 
	{ 
		ST7735_DrawString(3, 11, "SHORTS &", ST7735_CYAN);
		ST7735_DrawString(3, 12, "LONG SLEEVES", ST7735_CYAN);
	}
	else if((weather_temp_int >= 60) && (weather_temp_int < 75)) 
	{ 
		ST7735_DrawString(3, 11, "PANTS &", ST7735_CYAN);
		ST7735_DrawString(3, 12, "SHORT SLEEVES", ST7735_CYAN);
	}
	else if((weather_temp_int >= 50) && (weather_temp_int < 60)) 
	{ 
		ST7735_DrawString(3, 11, "PANTS &", ST7735_CYAN);
		ST7735_DrawString(3, 12, "LONG SLEEVES", ST7735_CYAN);
	}
	else if((weather_temp_int >= 35) && (weather_temp_int < 50)) 
	{
		ST7735_DrawString(3, 11, "JACKET OR HOODIE", ST7735_CYAN);
	}
	else if(weather_temp_int < 35) 
	{
		ST7735_DrawString(5, 11, "COAT", ST7735_CYAN);
	}
	else {ST7735_DrawString(5, 11, "ERROR", ST7735_CYAN);}
	
	ST7735_DrawString(0, 14, "HAVE A NICE DAY! :D", ST7735_WHITE);
}

//Display Calendar
//Display Month and Day In Standard Calendar Grid
//Grid for the Day will be marked red, else black
//Input: SlidePot
void Display_PG2(void)
{
	ST7735_DrawString(4, 0, "MARCH 2016", ST7735_GREEN);	//substitute with pulled data
	
	ST7735_DrawString(0, 4, "SUN", ST7735_GREEN);
	ST7735_DrawString(3, 4, "MON", ST7735_WHITE);
	ST7735_DrawString(6, 4, "TUE", ST7735_YELLOW);
	ST7735_DrawString(9, 4, "WED", ST7735_CYAN);
	ST7735_DrawString(12, 4, "THU", ST7735_WHITE);
	ST7735_DrawString(15, 4, "FRI", ST7735_YELLOW);
	ST7735_DrawString(18, 4, "SAT", ST7735_CYAN);
	
	ST7735_Line(0, 50, 128, 50, ST7735_WHITE);
	ST7735_Line(0, 70, 128, 70, ST7735_WHITE);
	ST7735_Line(0, 90, 128, 90, ST7735_WHITE);
	ST7735_Line(0, 110, 128, 110, ST7735_WHITE);
	ST7735_Line(0, 130, 128, 130, ST7735_WHITE);
	ST7735_Line(0, 150, 128, 150, ST7735_WHITE);
	
	ST7735_Line(0, 50, 0, 150, ST7735_WHITE);
	ST7735_Line(18, 50, 18, 150, ST7735_WHITE);
	ST7735_Line(36, 50, 36, 150, ST7735_WHITE);
	ST7735_Line(54, 50, 54, 150, ST7735_WHITE);
	ST7735_Line(72, 50, 72, 150, ST7735_WHITE);
	ST7735_Line(90, 50, 90, 150, ST7735_WHITE);
	ST7735_Line(108, 50, 108, 150, ST7735_WHITE);
	ST7735_Line(127, 50, 127, 150, ST7735_WHITE);
	
	for(int i=0; i<31; i++)
	{
		
	}
}

//Display important news etc
//Input: SlidePot, Update SW4
void Display_PG3(void)
{
}

//Display Stock, Currency Exchange rate, Gas Price
//Input: Slidepot, Update SW4
void Display_PG4(void)
{
}


//Convert String to Integer
int Convert_StringToInteger(const char* str)
{
	return 0;
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


//LAB3 ANALOG CLOCK CODE

/*
void DisplaySecond(void)
{
	volatile static double x, y;
	x = 0;
	y = 0;
	x = CENTER_X + LENGTH_S*cos(Time_Seconds*6*(PI/180)-1.58);
	y = CENTER_Y + LENGTH_S*sin(Time_Seconds*6*(PI/180)-1.58);
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
	volatile static double x, y;
	x = 0;
	y = 0;
	x = CENTER_X + LENGTH_M*cos(Time_Minutes*6*(PI/180)-1.58);
	y = CENTER_Y + LENGTH_M*sin(Time_Minutes*6*(PI/180)-1.58);
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
	volatile static double x, y;
	x = 0;
	y = 0;
	x = CENTER_X + LENGTH_H*cos(Time_Hours*30*(PI/180)-1.58);
	y = CENTER_Y + LENGTH_H*sin(Time_Hours*30*(PI/180)-1.58);
	prevhx = x;
	prevhy = y;
	
	ST7735_Line(CENTER_X, CENTER_Y, x, y, ST7735_WHITE);
}

void EraseHour(void)
{
	ST7735_Line(CENTER_X, CENTER_Y, prevhx, prevhy, ST7735_BLACK);
}

void ChooseMode(void)
{
	if (display_mode==1) {DisplayAnalog();}
	else if(display_mode==0) {DisplayDigital();}
	else {}
}


void DisplayAnalog(void)
{int32_t timeDisplay;
	timeDisplay=NVIC_ST_CURRENT_R;
		Output_Clear();
		ST7735_SetCursor(0, 0);
		ST7735_OutString("alarm:");
		ST7735_OutUDec(alarm_hours);
		ST7735_OutString(":");
		ST7735_OutUDec(alarm_minutes);
		
		ClockFace_Init();
		DisplaySecond();
		DisplayMinute();
		DisplayHour();
		timeDisplay=timeDisplay-NVIC_ST_CURRENT_R;
		while(active_In10s)
		{
			timeDisplay=NVIC_ST_CURRENT_R;
			if(Mode == 3)
			{
				break;
			}
			if(displayFlag == 0x01) 
			{
				displayFlag = 0xFF;
				EraseSecond();
				DisplaySecond();
			}
			if(displayFlag == 0x03) 
			{
				displayFlag = 0xFF;
				EraseSecond();
				DisplaySecond();
				EraseMinute();
				DisplayMinute();
			}
			if(displayFlag == 0x07) 
			{
				displayFlag = 0xFF;
				EraseSecond();
				DisplaySecond();
				EraseMinute();
				DisplayMinute();
				EraseHour();
				DisplayHour();
			}
			timeDisplay=NVIC_ST_CURRENT_R-timeDisplay;
			timeDisplay++;
		}	
}*/

/*
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
*/

