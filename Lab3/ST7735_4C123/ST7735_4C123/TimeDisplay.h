// TimeDisplay.c
// TM4C123

//------------Switch_Init------------
// Initialize GPIO Port A bit 5 for input
// Input: none
// Output: none
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
