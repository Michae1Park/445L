// Switch.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize a GPIO as an input pin and
// allow reading of two negative logic switches on PF0 and PF4
// and an external switch on PA5.
// Use bit-banded I/O.
// Daniel and Jonathan Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Section 4.2    Program 4.2

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Example 2.3, Program 2.9, Figure 2.36

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// negative logic switches connected to PF0 and PF4 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad
// NOTE: The NMI (non-maskable interrupt) is on PF0.  That means that
// the Alternate Function Select, Pull-Up Resistor, Pull-Down Resistor,
// and Digital Enable are all locked for PF0 until a value of 0x4C4F434B
// is written to the Port F GPIO Lock Register.  After Port F is
// unlocked, bit 0 of the Port F GPIO Commit Register must be set to
// allow access to PF0's control registers.  On the LM4F120, the other
// bits of the Port F GPIO Commit Register are hard-wired to 1, meaning
// that the rest of Port F can always be freely re-configured at any
// time.  Requiring this procedure makes it unlikely to accidentally
// re-configure the JTAG and NMI pins as GPIO, which can lock the
// debugger out of the processor and make it permanently unable to be
// debugged or re-programmed.
#include <stdint.h>
#include "../Shared/tm4c123gh6pm.h"
#include "SysTick.h"
#include "Switch.h"
#include "SetTime.h"
#include "SetAlarm.h"
#include "Display.h"
#include "ST7735.h"
#include "Timer0A.h"
#include "ST7735.h"

#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define PF0                     (*((volatile uint32_t *)0x40025004))
#define PF4                     (*((volatile uint32_t *)0x40025040))
#define PA5                     (*((volatile uint32_t *)0x40004080))
#define SWITCHES                (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control\


#define MENU_NONE							0x00
#define MENU_SET_TIME					0x01
#define	MENU_SET_ALARM				0x02
//#define MENU_DISPLAY_TIME			0x03
//#define MENU_ALARM						0x04

volatile uint16_t menu_mode;
volatile uint16_t hours2,minutes2;

volatile uint16_t Mode;

int last0=1;
int last1=1;
int last2=1;
int last3=1;

//------------Switch_Init------------
// Initialize GPIO Port D 
// Input: none
// Output: none
void Switch_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate clock for Port D
	int i;
	i=SYSCTL_RCGCGPIO_R;
  GPIO_PORTD_DIR_R &= ~0x0F;        // PD0-3 is an input
  GPIO_PORTD_AMSEL_R &= ~0x0F;      // disable analog on PD0-3
  GPIO_PORTD_DEN_R |= 0x0F;         // PD0-3 enabled as a digital port
	GPIO_PORTD_IS_R &= ~0x0F;						// PD 0-3 is edge-sensitive
	GPIO_PORTD_IBE_R &= ~0x0F;					// PD 0-3 is not both edges
	GPIO_PORTD_IEV_R &= ~0x0F;					// Pd 0-3 falling edge event
	GPIO_PORTD_ICR_R = 0x0F;						// clear flag 0-3
	GPIO_PORTD_IM_R |= 0x0F;						// arm interrupt on PD 0-3
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00A00000; // (5) priority 5
	NVIC_EN0_R = 0x00000008; 						//enable interrupt 1(PB) in NVIC
	SYSCTL_RCGCTIMER_R |= 0x08;
}

void Timer3_ARM(void){
	
  TIMER3_CTL_R = 0x00000000;    // 1) disable TIMER3A during setup
  TIMER3_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER3_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER3_TAILR_R = 20000000;    // 4) reload value
  TIMER3_TAPR_R = 0;            // 5) bus clock resolution
  TIMER3_ICR_R = 0x00000001;    // 6) clear TIMER3A timeout flag
  TIMER3_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 51, interrupt number 35
  NVIC_EN1_R = 1<<(35-32);      // 9) enable IRQ 35 in NVIC
  TIMER3_CTL_R = 0x00000001;    // 10) enable TIMER3A
	
	/*
  TIMER1_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000001;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = 20000000;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 23 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable timer2A*/
}

void GPIO_ARM(void){
	GPIO_PORTD_ICR_R = 0x0F;						// clear flag 0-3
	GPIO_PORTD_IM_R |= 0x0F;						// arm interrupt on PD 0-3
		NVIC_EN0_R = 0x00000008; 						//enable interrupt 1(PB) in NVIC
}

void Timer3A_Handler(void){
  TIMER3_IMR_R = 0x00000000;    // 7) arm timeout interrupt
	GPIO_ARM();
	last0=GPIO_PORTD_DATA_R & 0x01;
	last1=GPIO_PORTD_DATA_R & 0x02;
	last2=GPIO_PORTD_DATA_R & 0x04;
	last3=GPIO_PORTD_DATA_R & 0x08;
	
}


/*
GIPOPortD_Handler
ISR for Switch interface: PB0-3
Input: None
Output: None

SetTime and SetAlarm button functionality:
PD0 = Exit (From time_set or alarm_set mode)
PD1 = decrement min
PD2 = increment min
PD3 = increment hours
	
*/
void GPIOPortD_Handler(void)
{
	GPIO_PORTD_IM_R &= ~0x0F;						// arm interrupt on PD 0-3
	if(display_status == PG1)
	{
		
		if (GPIO_PORTD_RIS_R & 0X01  && last0) //poll PD0
		{
			GPIO_PORTD_ICR_R = 0x01; //acknowledge flag1 and clear
			//ST7735_DrawString(0, 9, "MENU NONE", ST7735_WHITE);
			if ((Mode == MENU_SET_ALARM) || (Mode ==MENU_SET_TIME))
			{
				//ST7735_DrawString(0, 9, "MENU NONE", ST7735_WHITE);
				Mode = MENU_NONE;
			}
			else 
			{
				//update PG1 data
			}
		}
		
		if (GPIO_PORTD_RIS_R & 0X02 && last1) //poll PD1
		{
			//ST7735_DrawString(0, 9, "Decrement Time", ST7735_WHITE);
			GPIO_PORTD_ICR_R = 0x02; //acknowledge flag1 and clear	
			
			if (Mode == MENU_SET_TIME)
			{
				decrementMin();
				Display_PG1();
			}
			else if (Mode == MENU_SET_ALARM)
			{
				decrementAlarmMin();
				Display_PG1();
			}
			else 
			{
				alarm_flag ^= 1;
			}
		}
		
		if (GPIO_PORTD_RIS_R & 0X04 && last2) //poll PD2
		{
			//ST7735_DrawString(0, 9, "Increment Time", ST7735_WHITE);
			GPIO_PORTD_ICR_R = 0x04; //acknowledge flag1 and clear

			if (Mode == MENU_SET_TIME)
			{
				incrementMin();
				Display_PG1();
			}
			else if (Mode == MENU_SET_ALARM)
			{
				incrementAlarmMin();
				Display_PG1();
			}
			else
			{
				Mode = MENU_SET_ALARM;
			}
		}
		
		if (GPIO_PORTD_RIS_R & 0X08 && last3) //poll PD3
		{
			//ST7735_DrawString(0, 9, "INCREMENT HOUR", ST7735_WHITE);
			GPIO_PORTD_ICR_R = 0x08; //acknowledge flag1 and clear
			
			if (Mode == MENU_SET_TIME)
			{
				incrementHour();
				Display_PG1();
			}
			else if (Mode == MENU_SET_ALARM)
			{
				incrementAlarmHour();
				Display_PG1();
			}
			else
			{
					//ST7735_DrawString(0, 9, "time-set mode********************************************", ST7735_WHITE);
				Mode = MENU_SET_TIME;
			}
		}	
	}
	else if (display_status == PG2)
	{
		//snooze, update
		if (GPIO_PORTD_RIS_R & 0X01 && last0) //poll PD0
		{
			GPIO_PORTD_ICR_R = 0x01; //acknowledge flag1 and clear
			//update info
		}
		if (GPIO_PORTD_RIS_R & 0X02 && last1) //poll PD1
		{
			GPIO_PORTD_ICR_R = 0x02; //acknowledge flag1 and clear
			alarm_flag ^= 1;
		}	
	}
	else if (display_status == PG3)
	{
		//snooze, update
		if (GPIO_PORTD_RIS_R & 0X01 && last0) //poll PD0
		{
			GPIO_PORTD_ICR_R = 0x01; //acknowledge flag1 and clear
			//update info
		}
		if (GPIO_PORTD_RIS_R & 0X02 && last1) //poll PD1
		{
			GPIO_PORTD_ICR_R = 0x02; //acknowledge flag1 and clear
			alarm_flag ^= 1;
		}	
	}
	else if (display_status == PG4)
	{
		//snooze, update
		if (GPIO_PORTD_RIS_R & 0X01 && last0) //poll PD0
		{
			GPIO_PORTD_ICR_R = 0x01; //acknowledge flag1 and clear
			//update info
		}
		if (GPIO_PORTD_RIS_R & 0X02 && last1) //poll PD1
		{
			GPIO_PORTD_ICR_R = 0x02; //acknowledge flag1 and clear
			alarm_flag ^= 1;
		}	
	}
	else
	{
		//ERROR
	}
	Timer3_ARM();
}

