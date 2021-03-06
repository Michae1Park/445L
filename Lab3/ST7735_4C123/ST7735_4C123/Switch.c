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
#include "TimeDisplay.h"
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
// Initialize GPIO Port A bit 5 for input
// Input: none
// Output: none
void Switch_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x02;        // 1) activate clock for Port B
 // while((SYSCTL_PRGPIO_R&0x02) == 0){};// ready?
  GPIO_PORTB_DIR_R &= ~0x0F;        // PB0-3 is an input
  //GPIO_PORTB_AFSEL_R &= ~0x0F;      // regular port function
  GPIO_PORTB_AMSEL_R &= ~0x0F;      // disable analog on PB0-3
  //GPIO_PORTB_PCTL_R &= ~0x0000FFFF; // PCTL GPIO on PB1 
  GPIO_PORTB_DEN_R |= 0x0F;         // PB3-0 enabled as a digital port
	GPIO_PORTB_IS_R &= ~0x0F;						// PB 0-3 is edge-sensitive
	GPIO_PORTB_IBE_R &= ~0x0F;					// PB 0-3 is not both edges
	GPIO_PORTB_IEV_R &= ~0x0F;					// PB 0-3 falling edge event
	GPIO_PORTB_ICR_R = 0x0F;						// clear flag 0-3
	GPIO_PORTB_IM_R |= 0x0F;						// arm interrupt on PB 0-3
	//NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00A00000; // (5) priority 5
	NVIC_EN0_R = 0x00000002; 						//enable interrupt 1(PB) in NVIC
}

//------------Switch_Input------------
// Read and return the status of GPIO Port A bit 5 
// Input: none
// Output: 0x20 if PA5 is high
//         0x00 if PA5 is low
uint32_t Switch_Input(void){
  return PA5; // return 0x20(pressed) or 0(not pressed)
}
uint32_t Switch_Input2(void){
  return (GPIO_PORTA_DATA_R&0x20); // 0x20(pressed) or 0(not pressed)
}

//------------Board_Init------------
// Initialize GPIO Port F for negative logic switches on PF0 and
// PF4 as the Launchpad is wired.  Weak internal pull-up
// resistors are enabled, and the NMI functionality on PF0 is
// disabled.
// Input: none
// Output: none
void Board_Init(void){            
  SYSCTL_RCGCGPIO_R |= 0x20;     // 1) activate Port F
  while((SYSCTL_PRGPIO_R&0x20) == 0){};// ready?
                                 // 2a) unlock GPIO Port F Commit Register
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
  GPIO_PORTF_CR_R |= (SW1|SW2);  // 2b) enable commit for PF4 and PF0
                                 // 3) disable analog functionality on PF4 and PF0
  GPIO_PORTF_AMSEL_R &= ~(SW1|SW2);
                                 // 4) configure PF0 and PF4 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFF0FFF0)+0x00000000;
  GPIO_PORTF_DIR_R &= ~(SW1|SW2);// 5) make PF0 and PF4 in (built-in buttons)
                                 // 6) disable alt funct on PF0 and PF4
  GPIO_PORTF_AFSEL_R &= ~(SW1|SW2);
//  delay = SYSCTL_RCGC2_R;        // put a delay here if you are seeing erroneous NMI
  GPIO_PORTF_PUR_R |= (SW1|SW2); // enable weak pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R |= (SW1|SW2); // 7) enable digital I/O on PF0 and PF4
}

//------------Board_Input------------
// Read and return the status of the switches.
// Input: none
// Output: 0x01 if only Switch 1 is pressed
//         0x10 if only Switch 2 is pressed
//         0x00 if both switches are pressed
//         0x11 if no switches are pressed
uint32_t Board_Input(void){
  return SWITCHES;
}

// Program 2.9 from Volume 2
#define PB0 (*((volatile uint32_t *)0x40005004))
#define PB1 (*((volatile uint32_t *)0x40005008))
#define PB2 (*((volatile uint32_t *)0x40005010))
#define PB3 (*((volatile uint32_t *)0x40005020))
//------------Switch_Init3------------
// Initialize GPIO Port B bit 0-3 for input
// Input: none
// Output: none
void Switch_Init3(void){
  SYSCTL_RCGCGPIO_R |= 0x02;        // 1) activate clock for Port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};// ready?
  GPIO_PORTB_DIR_R &= ~0x0F;        // PB0-3 is an input
  //GPIO_PORTB_AFSEL_R &= ~0x0F;      // regular port function
  //GPIO_PORTB_AMSEL_R &= ~0x0F;      // disable analog on PB0-3
  //GPIO_PORTB_PCTL_R &= ~0x0000FFFF; // PCTL GPIO on PB1 
  GPIO_PORTB_DEN_R |= 0x0F;         // PB3-0 enabled as a digital port
	GPIO_PORTC_IS_R &= ~0x0F;						// PB 0-3 is edge-sensitive
	GPIO_PORTC_IBE_R &= ~0x0F;					// PB 0-3 is not both edges
	GPIO_PORTC_IEV_R &= ~0x0F;					// PB 0-3 falling edge event
	GPIO_PORTC_ICR_R = 0x0F;						// clear flag 0-3
	GPIO_PORTC_IM_R |= 0x0F;						// arm interrupt on PB 0-3
	//NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00A00000; // (5) priority 5
	NVIC_EN0_R = 0x00000002; 						//enable interrupt 1(PB) in NVIC
}


//------------Switch_Input3------------
// Read and return the status of GPIO Port B bit 1 
// Input: none
// Output: 0x02 if PB1 is high
//         0x00 if PB1 is low
/*
uint32_t Switch_Input3(void){ 
	//need to set which bit triggers what case
	switch(menu_mode)
	{
		case MENU_MAIN:
			mainMenu();
			break;
		case MENU_SET_TIME:
			setTime();
			break;
		case MENU_SET_ALARM:
			setAlarm();
			break;
		case MENU_DISPLAY_TIME:
			displayTime();
			break;
		case MENU_ALARM:
			displayAlarm();
			break;
	}
	
  return PB1;      // 0x02 if pressed, 0x00 if not pressed
}

void mainMenu(void){//check to see which button is pressed in which method
	if(PB0){
		menu_mode=MENU_SET_TIME;
	//	setTime();
	}
	if(PB1){
		menu_mode=MENU_SET_ALARM;
	//	setAlarm();
	}
	if(PB2){
		menu_mode=MENU_DISPLAY_TIME;
//		displayTime();
	}
	if(PB3){
		menu_mode=MENU_ALARM;
	//	displayAlarm();
	}
}



void setTime(void){//time set method
		if(PB0){
		//setTimeInit();
		//setTimeBase(0);
	}
	if(PB1){
		setTimeInit();
		setTimeBase(1);
	}
	if(PB2){
		setTimeInit();
		setTimeBase(2);
	}
	if(PB3){
		setTimeInit();
		setTimeBase(3);
	}
	if(Switch_Debounce()){
		menu_mode=MENU_MAIN;
		mainMenu();
	}
}
void setAlarm(void){
	if(PB0){
		setAlarmTimeInit();
		setAlarmTimeBase(0);
	}
	if(PB1){
		setAlarmTimeInit();
		setAlarmTimeBase(1);
	}
	if(PB2){
		setAlarmTimeInit();
		setAlarmTimeBase(2);
	}
	if(PB3){
		setAlarmTimeInit();
		setAlarmTimeBase(3);
	}
	if(Switch_Debounce()){
		menu_mode=MENU_MAIN;
		mainMenu();
	}
	

	
}
void displayTime(void){
		if(PB0){
			//SwitchModes();
			//DisplayRefresh(hours2, minutes2);
	}
	
	
	
}
void displayAlarm(void){
		if(PB0){
		AlarmToggleOFF();
	}
	if(PB1){
		AlarmToggleON();
	}
	
	
	
}


*/





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

  
