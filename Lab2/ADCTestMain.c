// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015

// ADCTestMain.c
// Michael Park, Jack Zhao
// Date Modified: 02/2/16
// Main file. Includes hardware averaging options, jitter calculation function, adc plot function, and draw line function.
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/8/16

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

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include "ADCSWTrigger.h"
#include "../Shared/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "Timer2.h"
#include <stdlib.h>
#include <stdio.h>
#include "ST7735.h"
#include "tm4c123gh6pm.h"


#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define DEBUG	//Debugging Feature for collecting 1000 samples. Uncomment to Enable the debugging feature

typedef int bool;
#define true 1
#define false 0

//********function prototypes*******************************************************************//
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void calcTimeDif(void);
void adcPMF(void);
void partG(void);
		
//*******global variables***********************************************************************//
volatile uint32_t ADCvalue;
uint32_t time_dump[1000], adc_dump[1000], sorted_ADC[1000], yBuffer[4096];
static uint32_t count=0;
volatile bool flag=false;

//**cmpfunc for qsort**************************//
int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}


// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 799999;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
	
	
#ifdef DEBUG 				//debugging feature for collecting 1000 samples
	if(count<1000){
		PF1 = (PF1*12345678)/1234567+0x02;  // this line causes jitter 
		adc_dump[count]=ADCvalue;
		time_dump[count] = TIMER1_TAR_R;
		count++;
	}
	else{	
		TIMER1_IMR_R &= 0x11111110; 
		TIMER0_IMR_R &= 0x11111110;
		flag=true;
	}
#endif
	
  PF2 ^= 0x04;                   // profile
	
}
int main(void)
{
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port 
	ST7735_InitR(INITR_REDTAB);
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
	Timer1_Init(0,12);
//	Timer2_Init(0,99000);
  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                      // turn off LED
	
//*****Hardware Averaging Option*********************//
	ADC0_SAC_R &= 0xFFFFFFF8;	//no hardware oversample  
	//ADC0_SAC_R &= 0xFFFFFFF8; //4x hardware oversample
	//ADC0_SAC_R |= 0x2; 
	//ADC0_SAC_R &= 0xFFFFFFF8;		//16x hardware oversample
	//ADC0_SAC_R |= 0x4; 
//	ADC0_SAC_R &= 0xFFFFFFF8; //64x hardware oversample
	//ADC0_SAC_R |= 0x6; 
					
  EnableInterrupts();

  while(1)
	{
    PF1 ^= 0x02;  // toggles when running in main
		//GPIO_PORTF_DATA_R ^= 0x02;    //toggles when running in main
		if(flag == true)
		{
			calcTimeDif();
			adcPMF();
			flag=false;
			partG();	
		}	
  }
}


//*********calcTimeDif function*********************
//This is used along with the debugging feature implemented inside Timer0A_Handler.
//This function calculates the jitter of collected ADC value by maxtime - mintime.
void calcTimeDif(void)
{
	uint32_t timeDif[999];
	uint32_t min=0, max=0;
	uint32_t jitter;
		uint32_t hi=0;
	
	for(int i =1;i<1000;i++){
		timeDif[i-1]=time_dump[i-1]-time_dump[i];
		if(timeDif[i-1]==0){
			hi++;
		}
	}
	min=timeDif[0];
	max=timeDif[0];
	for(int j=1;j<999;j++){
		if(timeDif[j]>max){
			max=timeDif[j];
		}
		if(timeDif[j]<min){
			min =timeDif[j];
		}
	}
	jitter=max-min;
	hi++;
}


//*********adcPMF function*********************
//This is used along with the debugging feature implemented inside Timer0A_Handler.
//This function calculates the frequency of each ADC value and outputs a PMF to ST7735 LCD screen.
void adcPMF(void){
//Function for building the PMF on the LCD
	
uint32_t counter=0;		//counter for the yBuffer
uint32_t range=0;
uint32_t maxfreq=0;
uint32_t maxfreqpos=0;

//sort ADC values
	for(int i =0;i<1000;i++){
		sorted_ADC[i]=adc_dump[i];
	}
	qsort(sorted_ADC, 1000, sizeof(uint32_t), cmpfunc);
	range=sorted_ADC[999]-sorted_ADC[0];
	
//create xBuffer and yBuffer
	for(int j=0; j<4096; j++) 
	{
		while( (sorted_ADC[counter]==j) && (counter<1000) )
		{
			yBuffer[j]++;
			counter++;
			if(yBuffer[j]>=maxfreq)
			{
				maxfreq=yBuffer[j];
				maxfreqpos=j;
			}
		}
	}
		
	//"PMF of ADC"
	ST7735_SetCursor(0,0); 
	ST7735_OutString("PMF of ADC");
	ST7735_SetCursor(0,159); 
	ST7735_PlotClear(32,159); 
		
	if(maxfreqpos<63)
	{
		for(int i =0; i<128; i++)
		{
			ST7735_PlotBar(yBuffer[i]);
			ST7735_PlotNext(); 
		}	
		count++;
	}
	else if(maxfreqpos>4033)
	{
		for(int i=4033-128;i<4033;i++)
		{
			ST7735_PlotBar(yBuffer[i]);
			ST7735_PlotNext(); 
		}	
	}
	else
	{
		for(int i =maxfreqpos-63; i<maxfreqpos+63; i++)
		{
			ST7735_PlotBar(yBuffer[i]);
			ST7735_PlotNext(); 
		}	
	}
	count++;
}

//*********partG function*********************
//This function allows to draw lines to ST7735 LCD screen in any direction and any length the user wants.
void partG(void)
{
	ST7735_SetCursor(0,0); 
	ST7735_OutString("Part G            ");
	ST7735_SetCursor(63,95); 
	ST7735_PlotClear(32,159); 
		
	ST7735_Line(0, 32, 127, 159, ST7735_BLUE);	
	ST7735_Line(0, 159, 127, 32, ST7735_BLUE);
}



