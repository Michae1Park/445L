
#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "TimeDisplay.h"
#include "SetTime.h"
#include "PLL.h"
#include "Timer1.h"
#include "Switch.h"
#include "SetAlarm.h"
#include "../Shared/tm4c123gh6pm.h"


#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define PERIOD					0x04C4B400

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


volatile uint16_t TimeDisplay_Mode, SetTime_Mode, SetAlarm_Mode, ToggleAlarm_Mode, MainMenu_Mode;
volatile uint16_t Time_Seconds, Time_Minutes, Time_Hours; 


int main(void){uint32_t j; // main 1
  PLL_Init(Bus80MHz);                  	// set system clock to 80 MHz
	Timer1_Init(0, PERIOD);								// Init Timer1 for global clock
  ST7735_InitR(INITR_REDTAB);						// Init PORTA and LCD initializations
	Switch_Init(); 												// Init PORTB and switch initialization
	//PWM_Init();														// Init PWM Sound module
	
	//PORTF INIT
	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
	PF2 = 0;                      				// turn off LED
	
	//USER FUNCTION INIT
	TimeDisplay_Init();										// Self Described Init Functions
	SetTime_Init();												
	//SetAlarm_Init();
	//ToggleAlarm_Init();
	//MainMenu_Init();
	
	EnableInterrupts();										// Enable Interrupts
	
  while(1) {
		if(TimeDisplay_Mode==1){
			DisplayRefresh();
		}
		if(SetTime_Mode==1){
			DisplaySetTime();
		}
		if(SetAlarm_Mode==1){
			
		}
		if(ToggleAlarm_Mode==1){
			
		}
		if(MainMenu_Mode==1){
			
		}
  }
}