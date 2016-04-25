// Switch.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize a GPIO as an input pin and
// allow reading of two negative logic switches on PF0 and PF4
// and an external switch on PA5.
// Use bit-banded I/O.
// Daniel and Jonathan Valvano
// September 12, 2013

// Switch.c
// Michael Park, Jack Zhao, Corey Cormier
// Last modified on April 20, 2016

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

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define PF0                     (*((volatile uint32_t *)0x40025004))
#define PF4                     (*((volatile uint32_t *)0x40025040))
#define PA5                     (*((volatile uint32_t *)0x40004080))
#define SWITCHES                (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

volatile uint32_t desiredRev;

int last0=1;
int last1=1;
//------------Switch_Init------------
// Initialize GPIO Port D 
// Input: none
// Output: none
void Switch_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate clock for Port D
	int i;
	i=SYSCTL_RCGCGPIO_R;
  GPIO_PORTD_DIR_R &= ~0x0C;        // PD0-3 is an input
  GPIO_PORTD_AMSEL_R &= ~0x0C;      // disable analog on PD0-3
  GPIO_PORTD_DEN_R |= 0x0C;         // PD0-3 enabled as a digital port
	GPIO_PORTD_IS_R &= ~0x0C;						// PD 0-3 is edge-sensitive
	GPIO_PORTD_IBE_R &= ~0x0C;					// PD 0-3 is not both edges
	GPIO_PORTD_IEV_R &= ~0x0C;					// Pd 0-3 falling edge event
	GPIO_PORTD_ICR_R = 0x0C;						// clear flag 0-3
	GPIO_PORTD_IM_R |= 0x0C;						// arm interrupt on PD 0-3
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00A00000; // (5) priority 5
	NVIC_EN0_R = 0x00000008; 						//enable interrupt 1(PB) in NVIC
	SYSCTL_RCGCTIMER_R |= 0x02;
	
}

uint32_t Switch_Input(void){
  return GPIO_PORTD_RIS_R &0x03; // status of 
}

void Timer1_ARM(void){
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
  TIMER1_CTL_R = 0x00000001;    // 10) enable timer2A
}

void GPIO_ARM(void){
	GPIO_PORTD_ICR_R = 0x0C;						// clear flag 0-3
	GPIO_PORTD_IM_R |= 0x0C;						// arm interrupt on PD 0-3
		NVIC_EN0_R = 0x00000008; 						//enable interrupt 1(PB) in NVIC
}

void Timer1A_Handler(void){
  TIMER1_IMR_R = 0x00000000;    // 7) arm timeout interrupt
	GPIO_ARM();
	last0=GPIO_PORTD_DATA_R & 0x04;
	last1=GPIO_PORTD_DATA_R & 0x08;
	
	
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


void GPIOPortD_Handler(void)
{
	
	GPIO_PORTD_IM_R &= ~0x0C;						// arm interrupt on PD 0-3
	//switch debounce
	//uint32_t newSwitch=	Switch_Debounce();
	//set functions
	//long sr=StartCritical();
		if(GPIO_PORTD_RIS_R & 0X04 && last0) //poll PD0
		{
			GPIO_PORTD_ICR_R = 0x01; //acknowledge flag1 and clear
			if(desiredRev + 5 < 40)
			{
				desiredRev += 5;
			}
			else
			{
				desiredRev = 40;
			}
		}
		
		if(GPIO_PORTD_RIS_R & 0X08  &&last1) //poll PD1
		{
			GPIO_PORTD_ICR_R = 0x02; //acknowledge flag1 and clear	
			if(desiredRev - 5 > 15)
			{
				desiredRev -= 5;
			}
			else
			{
				desiredRev = 15;
			}

		}
		Timer1_ARM();
		//EndCritical(sr);
}
