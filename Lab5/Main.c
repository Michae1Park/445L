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
#include "DAC.h"
#include "SysTick.h"
#include "PLL.h"
#include "Notes.h"
#include "Switch.h"

volatile uint32_t isPlaying;
volatile uint32_t isFast;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

int main(void)
{
	PLL_Init(Bus50MHz);              // bus clock at 50 MHz
	Switch_Init();
	
	/*
	SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
	while((SYSCTL_PRGPIO_R&0x0020) == 0){ };// ready?
	GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
	GPIO_PORTF_AFSEL_R &= ~0x0E;     // disable alt funct on PF3-1
	GPIO_PORTF_DEN_R |= 0x0E;        // enable digital I/O on PF3-1
																	 // configure PF3-1 as GPIO
	GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
	GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
	LEDS = 0;                        // turn all LEDs off
	SysTick_Init(160000000);					//Tempo set to 60 bpm (interrupt every second)
	Timer0A_Init(&UserTask, (Song[note] / 32));  // initialize timer0A to 440 Hz * 32 (to account for the size of the waveform table)
	*/
	
	DAC_Init(2056);
	EnableInterrupts();

  while(1)
	{
 
  }
}
