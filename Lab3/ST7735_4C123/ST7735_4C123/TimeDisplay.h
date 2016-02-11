// TimeDisplay.c
// TM4C123

//------------Switch_Init------------
// Initialize GPIO Port A bit 5 for input
// Input: none
// Output: none
void TimeDisplay_Init(void);
void DisplayMinute(uint32_t minutes);
void DisplayHour(uint32_t hours);
void DisplayDigital(uint32_t hours, uint32_t minutes);

void DisplayRefresh(uint32_t hours, uint32_t minutes);
void SwitchModes(void);

