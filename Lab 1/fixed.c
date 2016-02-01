//Michael Park(mjp2853), Jack Zhao
//SP16 01/26/16

#include <stdint.h>
#include <stdio.h>
#include "fixed.h"
#include "ST7735.h"

//global variables shared by the plotting methods (3,4)
int32_t Y_min, X_min, Y_max, X_max, res;  

//The purpose of this technique is to use integer arithmetic (int, long...) while being able to represent fractions
void ST7735_sDecOut3(int32_t n)
{
	
  uint8_t flag = 0;
  char ch[6];
  for (int i=0; i<6; i++)
  {
    ch[i] = '0';
  }
	
	//n = n*1000*0.001;
	
	if (n > 9999 || n < -9999)
	{
		printf(" *.*** ");
	}
	else
	{
		sprintf(ch, "%.5d", n);
		for (int i=0; i<6; i++)
		{
			//negatives
			if(ch[0] == '-')
			{
				if(i==3)
				{
					fputc('.', 0);
					fputc(ch[i], 0);
				}
				else if (i==1 && ch[i]=='0')
				{
					flag = 1;					
					continue;
				}
				else
					fputc(ch[i], 0);
			}
			
			//positives
			else
			{
				if(i==2)
				{
					fputc('.', 0);
					fputc(ch[i], 0);
				}
				else if(ch[i] == '0' && i==0)	//replacing the most significant digit with space
				{
					fputc(' ' , 0);
					ch[i] = ' ';
				}
				else
					fputc(ch[i], 0);
			}
		}
		
		if(flag) //if skipped one digit for negative values
		{
				flag = 0;
			fputc(' ', 0);
		}
	}
}

void ST7735_uBinOut8(uint32_t n)
{
	uint32_t n1 = n*1000/256;		//integer part
	uint32_t n2 = n1%10;
	char ch[6];
	for (int i=0; i<6; i++)
	{
		ch[i] = '0';
	}
	
	if (n2 >= 5) //version 2 = rounding up to the second decimal digit
	{
		n1 = n1/10;
		n1++;
	}
	
	//n1 = n1/10; //version 1 = discarding digits below second decimal
			
	//build string and then output
	if (n1 > 99999) //error case
	{
		printf("***.** ");
	}
	else
	{
		sprintf(ch, "%.5d", n1);
		for (int i=0; i<6; i++)	
		{
			if (i == 3)	//putting a dot
			{
				fputc('.', 0);
				fputc(ch[i], 0);	
			}
			else if(ch[i] == '0' && i==0)	//replacing the most significant digit with space
			{
				fputc(' ' , 0);
				ch[i] = ' ';
			}
			else if((ch[i-1]==' ') && (ch[i]=='0')) //replacing second most significant digit with space
			{
				fputc(' ' , 0);
			}
			else
				fputc(ch[i], 0);	
		}
	}
}

//y: 0-159, x: 0-127
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY)
{
	Output_Clear();
	
	char* graphtitle  = title; 
	res = 100;
	
	Y_max = (maxY - minY)/res;
	Y_min = minY/res;
	X_max = (maxX - minX)/res;
	X_min = minX/res;
	
	if(Y_max >127) Y_max =127;
	if(X_max >127) X_max =127;
	
	ST7735_DrawString(0, 0, graphtitle, ST7735_YELLOW);
	ST7735_FillRect(0, 32, X_max, Y_max, ST7735_Color565(228,228,228));
}


/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
 LCD: 128y*96x
*/

void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[])
{
	int32_t x;
	int32_t y;
	for(int i=0;i<num;i++)
	{
		x=bufX[i]/res;
		y=bufY[i]/res;

		int32_t yplot = (y - Y_min)+32;
		int32_t xplot = (x - X_min);
		
		if(xplot<0) yplot = 0;
		if(xplot>X_max) xplot = X_max;
		if(yplot<32) yplot = 32;
		if(yplot>Y_max+32) yplot = Y_max+32;
		
		ST7735_DrawPixel(xplot,   yplot,   ST7735_BLUE);
		ST7735_DrawPixel(xplot+1, yplot,   ST7735_BLUE);
		ST7735_DrawPixel(xplot,   yplot+1, ST7735_BLUE);
		ST7735_DrawPixel(xplot+1, yplot+1, ST7735_BLUE);
	}
}

/***********************EXTRA CREDIT***************************/
//version 1
//Time took to exectue: 2543238*12.5ns = 31.790475 ms
volatile float T;    // temperature in C
volatile uint32_t N; // 12-bit ADC value
void Test1EC(void){
  for(N=0; N<4096; N++){
    T = 10.0+ 0.009768*N; 	
  }
}

// version 2: C fixed-point
//Time took to exectue: 163869*12.5ns = 2.0483625 ms
volatile uint32_t T2;    // temperature in 0.01 C
volatile uint32_t N;    // 12-bit ADC value
void Test2EC(void){
  for(N=0; N<4096; N++){
    T2 = 1000 + (125*N+64)>>7; 	
  }
}