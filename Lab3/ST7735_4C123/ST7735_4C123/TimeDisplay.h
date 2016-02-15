// TimeDisplay.c
// TM4C123

//------------Switch_Init------------
// Initialize GPIO Port A bit 5 for input
// Input: none
// Output: none

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
