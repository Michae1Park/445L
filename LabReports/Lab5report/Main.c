// Main.c
// Michael Park, Jack Zhao
// Date Created: 02/24/2016
// initialize pll, heartbeat, switch, timer 
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/29/2016

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


int main(void)
{
	PLL_Init(Bus50MHz);              // bus clock at 50 MHz
	PORTF_Init();
	Switch_Init();
	SysTick_Init(50000000);					//Tempo set to 1 bit/sec	
	Timer0A_Init(&PlaySong, (Song3[note] / 32));  // initialize timer0A to 440 Hz(A4) * 32(Size of Wave) 
	Timer1_Init(&PlaySong2, (Song2[note] / 32));
	Timer2_Init(&addWaves, 500);
	
	DAC_Init(2048);
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





