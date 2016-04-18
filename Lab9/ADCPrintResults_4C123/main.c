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

//uint16_t const ADCTable[53]={0,34,66,100,135,171,208,246,286,328,370,415,460,507,556,607,659,713,769,827,887,949,1013,1079,1148,1218,1291,1366,1444,1524,1607,
//	     1693,1781,1872,1966,2063,2162,2265,2370,2479,2591,2706,2824,2945,3070,3198,3329,3463,3601,3742,3886,4033,4096};

uint16_t const ADCTable[53]={0,184,226,269,313,358,404,452,501,550,601,							
     654,707,762,818,876,935,995,1057,1120,1184,							
     1251,1318,1388,1459,1531,1605,1681,1759,1838,1919,							
     2002,2086,2173,2261,2351,2443,2537,2632,2730,2829,							
     2931,3034,3140,3247,3356,3467,3580,3695,3812,3931,4052,4096};		

uint16_t const TempTable[53]={4000,4000,3940,3880,3820,3760,3700,3640,3600,3550,3510,		
     3510,3500,3490,3480,3470,3450,3400,3350,3300,3250,		
     3200,3000,2800,2680,2560,2450,2380,2370,2369,2350,		
     2330,2300,2280,2270,2260,2200,1840,1780,1720,1660,		
     1600,1540,1480,1420,1360,1300,1240,1180,1120,1060,1000,1000};		

//uint16_t const TempTable[53]={4000,4000,3920,3840,3760,3680,3600,3520,3440,3360,3280,3200,3120,3040,2960,2880,2800,2720,2640,2560,2480,2400,2320,2240,2160,2080,2000,1920,1840,1760,1680,
//	     1600,1520,1440,1360,1280,1200,1120,1040,960,880,800,720,640,560,480,400,320,240,160,80,0,0};

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

