// ATimeDisplay.c
// Michael Park, Jack Zhao
// Date Created: 2/12/2016
// Includes prototypes of analog and digital time display functions
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 2/15/2016


#include <stdint.h>

extern volatile uint16_t display_mode;

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
void DisplayAnalog(void);
void DisplayDigital(void);
