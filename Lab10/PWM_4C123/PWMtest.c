// PWMtest.c
// Runs on TM4C123
// Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs.
// Daniel Valvano
// March 28, 2014

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Program 6.8, section 6.3.2

   "Embedded Systems: Real-Time Operating Systems for ARM Cortex M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2015
   Program 8.4, Section 8.3

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
 
 	
	
//  PWM0_Duty(4000);    // 10%
//  PWM0_Duty(10000);   // 25%
//  PWM0_Duty(30000);   // 75%

//  PWM0_Init(4000, 2000);         // initialize PWM0, 10000 Hz, 50% duty
//  PWM0_Init(1000, 900);          // initialize PWM0, 40000 Hz, 90% duty
//  PWM0_Init(1000, 100);          // initialize PWM0, 40000 Hz, 10% duty
//  PWM0_Init(40, 20);             // initialize PWM0, 1 MHz, 50% duty


#include <stdint.h>
#include <stdio.h>
#include "PLL.h"
#include "PWM.h"
#include "Switch.h"
#include "PeriodMeasure.h"
#include "ST7735.h"
#include "../Shared/tm4c123gh6pm.h"



#define Timer2Period 1600000


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

volatile uint32_t PWMPeriod=40000;
volatile uint32_t count=0;
extern volatile uint32_t currentSpeed;

int main(void){
	DisableInterrupts();
	currentDuty=30000;
	desiredRev=30;
  PLL_Init(Bus80MHz);               // bus clock at 80 MHz
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_WHITE);    
  ST7735_FillRect(0, 0, 128, 50, ST7735_BLACK);
	ST7735_PlotClear(0, 4000);
	Switch_Init();
  PWM0B_Init(PWMPeriod, currentDuty);         // initialize PWM0, 1000 Hz, 25% duty
	//PWM0B_Off();
	PeriodMeasure_Init();
	Timer2_Init(0, Timer2Period);
	EnableInterrupts();
	newDuty=currentDuty;

  while(1){
		count++;
		if(count>=1000){
		//ST7735_PlotPoint(currentSpeed);
		ST7735_SetCursor(0,0);
		ST7735_OutString("Desired RPS: ");
    ST7735_OutUDec(desiredRev);
		ST7735_SetCursor(0,2);
		ST7735_OutString("Current RPS: ");
		ST7735_OutUDec(currentSpeed);
			
		ST7735_PlotNextErase();
		ST7735_PlotPoint(currentSpeed*100);
		ST7735_PlotNext();
		ST7735_PlotNextErase();
			
			count=0;
		}
		
  }
}


