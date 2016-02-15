// AlarmClockMain.c 
// Michael Park, Jack Zhao
// Date Created: 2/12/2016
// Driver for Alarm clock. Includes interrupts used
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 2/15/2016

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
#include "PWM.h"
#include "SysTickInts.h"
#include "Common.h"
#include "TimeDisplay.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define PERIOD					0x04C4B400
#define Tensec					0x2FAF0800	//number of cycle for 10s when each cycle = 12.5ns

enum ModeSetting
{  
	SetTime_Mode, 
	SetAlarm_Mode, 
	ToggleAlarm_Mode, 
	TimeDisplay_Mode
};  

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void MainMenu(void);

volatile uint16_t Mode;
volatile uint16_t active_In10s = 1;
volatile uint32_t counts;
volatile uint32_t timeout=0;

int main(void)
{
	//system component setup
	PLL_Init(Bus80MHz);                  	// set system clock to 80 MHz
	Timer1_Init(0, PERIOD);							// Init Timer1 for global clock
  ST7735_InitR(INITR_REDTAB);						// Init PORTA and LCD initializations
	Switch_Init(); 												// Init PORTB and switch initialization
	PWM0A_Init(40000, 30000);							// Init PWM Sound module
	SysTick_Init(80000);
	
	//PORTF INIT (For Heart Beat)
  SYSCTL_RCGCGPIO_R |= 0x20;  // activate port F
  GPIO_PORTF_DIR_R |= 0x04;   // make PF2 output (PF2 built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;// disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;   // enable digital I/O on PF2
                              // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;     // disable analog functionality on PF
	
	//USER FUNCTION INIT							// Self Described Init Functions									
	MainMenu();
	counts = 0;
	Mode = 0xFFFF;
	SysTick_Init(80000);        		// initialize SysTick timer
	EnableInterrupts();							// Enable Interrupts

	//Main Loop
  while(1) 
  {	
		if((alarm_hours==Time_Hours) && (alarm_minutes==Time_Minutes) && alarm_flag)
		{
			//enable PWM
		}
		else if((alarm_hours==Time_Hours) && (alarm_minutes==Time_Minutes) && (!alarm_flag))
		{
			//disable PWM
		}
		
		if(Mode == SetTime_Mode)
		{
			Mode = 0xFFFF;	//Acknowledge mode
			active_In10s = 1;
			changeTime();
			MainMenu();
		}
		if(Mode == SetAlarm_Mode)
		{
			Mode = 0xFFFF;
			active_In10s = 1;
			setAlarmTimeBase();
			MainMenu();
		}
		if(Mode == ToggleAlarm_Mode)
		{
			Mode = 0xFFFF;
			active_In10s = 1;
			alarm_flag ^= 0x1;
		}
		if(Mode == TimeDisplay_Mode)
		{
			Mode = 0xFFFF;
			active_In10s = 1;
			display_mode ^= 0x1;
			ChooseMode();
			MainMenu();
		}
  }
}

/*
Initializes to print the Main Menu
*/
void MainMenu(void)
{
	Output_Clear();
	ST7735_DrawString(0, 0, "Set Time", ST7735_YELLOW);
	ST7735_DrawString(0, 3, "Set Alarm", ST7735_YELLOW);
	ST7735_DrawString(0, 6, "Turn On/Off Alarm", ST7735_YELLOW);
	ST7735_DrawString(0, 9, "Display Mode", ST7735_YELLOW);
}

// Interrupt service routine
// Executed every 12.5ns*(period) = 1ms
void SysTick_Handler(void)
{
  counts = counts + 1;
	if((counts % 1000) == 0)	//every 1s
	{
		PF2 ^= 0x04;                // toggle PF2
		timeout += 1;
	}
	
	if(timeout == 10)
	{
		active_In10s = 0;
		timeout = 0;
	}
}

/*
GIPOPortB_Handler
ISR for Switch interface: PB0-3
Input: None
Output: None
*/
void GPIOPortB_Handler(void)
{
	//Debouncer
	Switch_Debounce();
	
	if (GPIO_PORTB_RIS_R & 0X01) //poll PB0
	{
		GPIO_PORTB_ICR_R = 0x01; //acknowledge flag1 and clear
		Mode = SetTime_Mode;
		timeout = 0;
	}
	if (GPIO_PORTB_RIS_R & 0X02) //poll PB1
	{
		GPIO_PORTB_ICR_R = 0x02; //acknowledge flag1 and clear	
		Mode = SetAlarm_Mode;
		timeout = 0;
	}
	if (GPIO_PORTB_RIS_R & 0X04) //poll PB2
	{
		GPIO_PORTB_ICR_R = 0x04; //acknowledge flag1 and clear
		Mode = ToggleAlarm_Mode;
		timeout = 0;
	}
	if (GPIO_PORTB_RIS_R & 0X08) //poll PB3
	{
		GPIO_PORTB_ICR_R = 0x08; //acknowledge flag1 and clear
		Mode = TimeDisplay_Mode;
		timeout = 0;
	}

}
	