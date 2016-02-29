// MAX549TestMain.c
// Runs on LM4F120/TM4C123
// Test the functions provided in MAX549.c by outputting
// a sine wave at a particular frequency.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

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
// Max549 pin 1 ground
// Max549 pin 2 OUTA
// Max549 pin 3 CS, SSI0Fss connected to PA3
// Max549 pin 4 DIN, SSI0Tx connected to PA5
// Max549 pin 5 SCLK SSI0Clk connected to PA2
// Max549 pin 6 OUTB
// Max549 pin 7 REF (cheap solution connects this to +3.3V)
// Max549 pin 8 +3.3V
#include <stdint.h>
#include <stdio.h>
#include "DAC.h"
#include "SysTickInts.h"
#include "PLL.h"
#include "Music.h"
#include "Switch.h"
#include "../Shared/tm4c123gh6pm.h"
#include "Timer0A.h"


#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define LEDS      (*((volatile uint32_t *)0x40025038))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void PORTF_Init(void);

const uint16_t Wave[32] = {1024,1122,1215,1302,1378,1440,1486,1514,1524,1514,1486,1440,1378,1302,1215,1122,1024,926,833,746,670,608,562,534,524,534,562,608,670,746,833,926};

unsigned char soundIndex = 0; //varies from 0 to 32

const uint32_t Song[128] = {A4, A4, A4, A4, A4, A4, B4, B4, C5, C5, E5, E5, E5, E5, C5, C5, B4, B4, G4, G4, G4, G4, F4, E4, F4, F4, F4, F4, F4, 0, 
	A4, C5, D5, D5, D5, D5, D5, D5, E5, E5, F5, F5, A4, A4, A4, A4, F5, F5, G5, G5, C5, C5, C5, C5, E5, F5, G5, G5, G5, G5, G5, G5, 0, 0,
	F5, F5, F5, F5, F5, F5, E5, E5, D5, D5, A5, A5, A5, A5, F5, F5, E5, E5, C5, C5, C5, C5, B_Flat4, A4, B_Flat4, B_Flat4, B_Flat4, B_Flat4, B_Flat4, 0, 
	D5, F5, G5, G5, G5, G5, G5, G5, A5, A5, B_Flat5, B_Flat5, D5, D5, D5, D5, B_Flat5, B_Flat5, C6, C6, F5, F5, F5, F5, A5, B_Flat5, C6, C6, C6, C6, C6, C6, 0, 0};
static uint16_t stop = 0;
int note = 0;

void PlaySong(void)
{
	PF1 ^= 0x02;
	DAC_Out(Wave[soundIndex]);
	PF1 ^= 0x02;
}

int main(void)
{
	PLL_Init(Bus50MHz);              // bus clock at 50 MHz
	PORTF_Init();
	Switch_Init();
	SysTick_Init(50000000);					//Tempo set to 1 bit/sec	
	Timer0A_Init(&PlaySong, (Song[note] / 32));  // initialize timer0A to 440 Hz * 32 (to account for the size of the waveform table)
	DAC_Init(2095);
	EnableInterrupts();
	while(1)
	{
		WaitForInterrupt();
	}
}

void PORTF_Init(void)
{
	SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
	while((SYSCTL_PRGPIO_R&0x0020) == 0){ };// ready?
	GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
	GPIO_PORTF_AFSEL_R &= ~0x0E;     // disable alt funct on PF3-1
	GPIO_PORTF_DEN_R |= 0x0E;        // enable digital I/O on PF3-1
																	 // configure PF3-1 as GPIO
	GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
	GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
	LEDS = 0;                        // turn all LEDs off
}

void SysTick_Handler(void)
{
  PF2 ^= 0x04;                // toggle PF2
  PF2 ^= 0x04;                // toggle PF2
	if(!stop)
	{
		note += 1;
		if(note > 127)
		{
			note = 0;
		}
		TIMER0_TAILR_R = (Song[note] / 32) - 1;
	}
	
  PF2 ^= 0x04;                // Debugging Profile
}


/*
GIPOPortB_Handler
ISR for Switch interface: PB0-2
Input: None
Output: None
*/
void GPIOPortB_Handler(void)
{
	if (GPIO_PORTB_RIS_R & 0X01) //poll PB0
	{
		GPIO_PORTB_ICR_R = 0x01; //acknowledge flag1 and clear
		stop = 0;
		//timeout = 0;
	}
	if (GPIO_PORTB_RIS_R & 0X02) //poll PB1
	{
		GPIO_PORTB_ICR_R = 0x02; //acknowledge flag1 and clear	
		stop = 1;
		//timeout = 0;
	}
	if (GPIO_PORTB_RIS_R & 0X04) //poll PB2
	{
		GPIO_PORTB_ICR_R = 0x04; //acknowledge flag1 and clear
		note = 0;
		//timeout = 0;
	}
}
