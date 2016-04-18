// ******* Main.c ************** 
// Michael Park, Jack Zhao
// Date Created: 04/09/2016
// Includes main functions
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 04/15/2016

// main.c
// Runs on LM4F120/TM4C123
// UART runs at 115,200 baud rate 
// Daniel Valvano
// May 3, 2015

/* This example accompanies the books
  "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
  ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

"Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
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


#include <stdio.h>
#include <stdint.h> // C99 variable types
#include "ADCSWTrigger.h"
#include "uart.h"
#include "PLL.h"
#include "ST7735.h"
#include "Timer1.h"
#include "Calib.h"


/* Assuming 80MHz Bus
1khz : 80000
100hz(Valvano Postulate) : 800000
500hz(Nyquist Theorom) :  160000
2khz(Aliased) :  40000 */
#define ADCfreq 80000

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void periodicADC(void);
int32_t ADCTempConvert(uint32_t ADC);
int32_t Interpolate(int lower, int higher, int32_t ADC);
void lcdFixed(int32_t temp);


volatile uint32_t counter;
volatile int32_t data[100]; 

int main(void){ 
	counter=0;
  PLL_Init(Bus80MHz);   // 80 MHz
  ST7735_InitR(INITR_REDTAB);					// Init PORTA and LCD initializations
  ST7735_FillScreen(ST7735_WHITE);    
  ST7735_FillRect(0, 0, 128, 50, ST7735_BLACK);
	ST7735_PlotClear(0, 4000);
	
  UART_Init();              // initialize UART device
  ADC0_InitSWTriggerSeq3_Ch9(4000000);//4000000
	
	Timer1_Init(*periodicADC,800 );//4000000
EnableInterrupts();	

	while(1){};
}

void periodicADC(void){
	uint32_t ADCval;
	uint32_t TEMPval;
	//ADCval=returnADC();
	ADCval=ADC0_InSeq3();
	//TEMPval=ADCTempConvert(ADCval);
	//UART_OutUDec(TEMPval);
	//prelab
	
	/*
	//data[counter]=TEMPval;
		data[counter]=ADC0_InSeq3();
	if(counter>=100){
		for(int k=0;k<20;k++){
			UART_OutString("\n\r");
		}
		UART_OutString("\n\r--------------------Starting--------------------");
		for(int j=0; j<100; j++){
			UART_OutString("\n\r");
			UART_OutUDec(j);
			UART_OutString("): ------ ADC data =");
			UART_OutUDec(data[counter]);
		}
		while(1){};
	}
	counter++;
	
	*/
	//end prelab

	TEMPval=ADCTempConvert(ADCval);
	lcdFixed(TEMPval);
	ST7735_SetCursor(0,2); 
	ST7735_OutString("ADC=");
	ST7735_OutUDec(ADCval);
	ST7735_PlotNextErase();
	ST7735_PlotPoint(TEMPval);
	ST7735_PlotNext();
	ST7735_PlotNextErase();
}
int32_t ADCTempConvert(uint32_t ADC){
	int point=0;
	while(ADCTable[point]<ADC){
		point++;
	}
	if(ADCTable[point]==ADC){
		return TempTable[point];
	}
	return Interpolate(point-1,point,ADC);
}

int32_t Interpolate(int lower, int higher, int32_t ADC){
	double tempLower=TempTable[lower];
	double tempHigher=TempTable[higher];
	double adcLower=ADCTable[lower];
	double adcHigher=ADCTable[higher];
	double currentADC=ADC;

	double result= tempLower+ (tempHigher-tempLower) * ((currentADC-adcLower)/(adcHigher-adcLower));
	return (int32_t) result;
}
void lcdFixed(int32_t temp){
	char out[6];
	ST7735_SetCursor(0,0); 
	ST7735_OutString("Thermometer");
	ST7735_SetCursor(0,1); 
	ST7735_OutString("T=");
	
	if(temp>99999){
		ST7735_DrawString(0,0,"***.**",ST7735_WHITE);
	}
	else{
		int before=temp/100;
		int after=temp%100;
		sprintf(out,"%d.%d",before,after);
		ST7735_OutString(out);
		ST7735_OutString(" C");
		
		
	}
}

