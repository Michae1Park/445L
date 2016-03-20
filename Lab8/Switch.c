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

#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define PF0                     (*((volatile uint32_t *)0x40025004))
#define PF4                     (*((volatile uint32_t *)0x40025040))
#define PA5                     (*((volatile uint32_t *)0x40004080))
#define SWITCHES                (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control\


#define MENU_MAIN							0x00
#define MENU_SET_TIME					0x01
#define	MENU_SET_ALARM				0x02
#define MENU_DISPLAY_TIME			0x03
#define MENU_ALARM						0x04

volatile uint16_t menu_mode;
volatile uint16_t hours2,minutes2;
//------------Switch_Init------------
// Initialize GPIO Port D 
// Input: none
// Output: none
void Switch_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate clock for Port D
	int i;
 // while((SYSCTL_PRGPIO_R&0x02) == 0){};// ready?
  GPIO_PORTD_DIR_R &= ~0x0F;        // PD0-3 is an input
  //GPIO_PORTB_AFSEL_R &= ~0x0F;      // regular port function
  GPIO_PORTD_AMSEL_R &= ~0x0F;      // disable analog on PD0-3
  //GPIO_PORTB_PCTL_R &= ~0x0000FFFF; // PCTL GPIO on PB1 
  GPIO_PORTD_DEN_R |= 0x0F;         // PD0-3 enabled as a digital port
	GPIO_PORTD_IS_R &= ~0x0F;						// PD 0-3 is edge-sensitive
	GPIO_PORTD_IBE_R &= ~0x0F;					// PD 0-3 is not both edges
	GPIO_PORTD_IEV_R &= ~0x0F;					// Pd 0-3 falling edge event
	GPIO_PORTD_ICR_R = 0x0F;						// clear flag 0-3
	GPIO_PORTD_IM_R |= 0x0F;						// arm interrupt on PD 0-3
	//NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00A00000; // (5) priority 5
	NVIC_EN0_R = 0x00000008; 						//enable interrupt 1(PB) in NVIC
}

//------------Switch_Input------------
// Read and return the status of GPIO Port A bit 5 
// Input: none
// Output: 0x20 if PA5 is high
//         0x00 if PA5 is low
uint32_t Switch_Input(void){
  return PA5; // return 0x20(pressed) or 0(not pressed)
}


#define DELAY10MS 160000
#define DELAY10US 160
//------------Switch_Debounce------------
// Read and return the status of the switch 
// Input: none
// Output: 0x02 if PB1 is high
//         0x00 if PB1 is low
// debounces switch
uint32_t Switch_Debounce(void){
uint32_t in,old,time; 
  time = 1000; // 10 ms
  old = Switch_Input();
  while(time){
    SysTick_Wait(DELAY10US); // 10us
    in = Switch_Input();
    if(in == old){
      time--; // same value 
    }else{
      time = 1000;  // different
      old = in;
    }
  }
  return old;
}

//------------Switch_Debounce------------
// wait for the switch to be touched 
// Input: none
// Output: none
// debounces switch
void Switch_WaitForTouch(void){
// wait for release
  while(Switch_Input()){};
  SysTick_Wait(DELAY10MS); // 10ms
// wait for touch
  while(Switch_Input()==0){};
  SysTick_Wait(800000); // 10ms
}

/*
GIPOPortD_Handler
ISR for Switch interface: PB0-3
Input: None
Output: None
*/
void GPIOPortD_Handler(void)
{
	//Debouncer
	Switch_Debounce();
	
	if (GPIO_PORTD_RIS_R & 0X01) //poll PD0
	{
		GPIO_PORTD_ICR_R = 0x01; //acknowledge flag1 and clear
	}
	if (GPIO_PORTD_RIS_R & 0X02) //poll PD1
	{
		GPIO_PORTD_ICR_R = 0x02; //acknowledge flag1 and clear	
	}
	if (GPIO_PORTD_RIS_R & 0X04) //poll PD2
	{
		GPIO_PORTD_ICR_R = 0x04; //acknowledge flag1 and clear
	}
	if (GPIO_PORTD_RIS_R & 0X08) //poll PD3
	{
		GPIO_PORTD_ICR_R = 0x08; //acknowledge flag1 and clear
	}	
}

