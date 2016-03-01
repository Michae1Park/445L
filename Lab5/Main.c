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
#include "Timer1.h"
#include "Timer2.h"


#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#define WHEELSIZE 8           // must be an integer multiple of 2

extern const uint32_t Song2[128];
extern const uint32_t Song[128];
extern const uint32_t Song3[128];

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void PORTF_Init(void);

volatile uint16_t stop = 0;
volatile uint16_t changeSound = 0;
extern volatile uint32_t note;



const long COLORWHEEL[WHEELSIZE] = {RED, RED+GREEN, GREEN, GREEN+BLUE, BLUE, BLUE+RED, RED+GREEN+BLUE, 0};

int main(void)
{
	PLL_Init(Bus50MHz);              // bus clock at 50 MHz
	PORTF_Init();
	Switch_Init();
	SysTick_Init(50000000);					//Tempo set to 1 bit/sec	
	Timer0A_Init(&PlaySong, (Song3[note] / 32));  // initialize timer0A to 440 Hz * 32 (to account for the size of the waveform table)
	Timer1_Init(&PlaySong2, (Song2[note] / 32));
	Timer2_Init(&addWaves, 500);
	
	DAC_Init(2095);
	Music_Init();
	//stop=1;
	EnableInterrupts();
	while(1)
	{
		WaitForInterrupt();
	}
}



//Initialize Port F
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





