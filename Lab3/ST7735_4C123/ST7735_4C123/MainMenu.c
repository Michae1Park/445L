// TimeDisplay.c
// TM4C123


#include <stdint.h>
#include <stdio.h>
#include "ST7735.h"
#include "MainMenu.h"
#include "ToggleAlarm.h"
#include "../Shared/tm4c123gh6pm.h"

extern long StartCritical (void);    // previous I bit, disable interrupts
extern void EndCritical(long sr);    // restore I bit to previous value
 
extern volatile uint16_t ToggleAlarm_Mode;


//------------TimeDisplay_Init------------
// Initialize GPIO Port A bit 5 for input
// Input: none
// Output: none
void MainMenu_Init(void){ 
	DisplayMainMenu();
}
void DisplayMainMenu(void){
	if(ToggleAlarm_Mode==1){
			CheckAlarmTrigger();
	}
	ST7735_SetCursor(0, 0);
	ST7735_OutString("1. Display Time");
	ST7735_SetCursor(0, 5);
	ST7735_OutString("2. Set Time");
	ST7735_SetCursor(0, 10);
	ST7735_OutString("3. Set Alarm");
	ST7735_SetCursor(0, 15);
	ST7735_OutString("4. Turn On/Off Alarm");
}



