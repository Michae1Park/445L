// ATimeDisplay.c
// Michael Park, Jack Zhao
// Date Created: 2/12/2016
// Includes prototypes of analog and digital time display functions
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 2/15/2016


#include <stdint.h>

#define ACTUALDATE 20

enum display_stat 
{
	PG1,
	PG2,
	PG3, 
	PG4
};

extern volatile uint16_t display_mode;
extern char weather_temp[3];
extern char weather_weather[6];
extern volatile uint16_t display_status;

void TimeDisplay_Init(void);
void DisplaySecond(void);
void EraseSecond(void);
void DisplayMinute(void);
void EraseMinute(void);
void EraseHour(void);
void DisplayHour(void);
void DisplayDigital(void);
void DisplayRefresh(void);
void SwitchMode(void);
void drawClockHands(void);
void ClockFace_Init(void);
void DelayWait10ms(uint32_t n);
void DisplayAlarm(void);
void DisplaySetTime(void);
void ChooseMode(void);
void Display_PG1(void);
void Display_PG2(void);
void Display_PG3(void);
void Display_PG4(void);
int Convert_StringToInteger(const char* str);

//Sunny, Rainy, Cloudy, Stormy, Snowy, 
//const char Weather_DB1 [5] = {'S', 'R', 'C', 'S', 'S'};
//const char Weather_DB2 [5] = {'u', 'a', 'l', 't', 'n'};
	
	