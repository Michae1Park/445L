// filename ******** fixed.c ************** 
// Michael Park, Jack Zhao
// Date Created: 01/26/2016
// Includes Fixed point calculation functions
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 01/31/2016

#include <stdint.h>
#include <stdio.h>
#include "fixed.h"
#include "ST7735.h"

//global variables shared by the plotting methods (3,4)
int32_t gb_ymin, gb_xmin, gb_ymax, gb_xmax, gb_res;  

/****************ST7735_sDecOut3***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.001
 range -9.999 to +9.999
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
 12345    " *.***"
  2345    " 2.345"  
 -8100    "-8.100"
  -102    "-0.102" 
    31    " 0.031" 
-12345    " *.***"
 */ 
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

/**************ST7735_uBinOut8***************
 unsigned 32-bit binary fixed-point with a resolution of 1/256. 
 The full-scale range is from 0 to 999.99. 
 If the integer part is larger than 256000, it signifies an error. 
 The ST7735_uBinOut8 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
     0	  "  0.00"
     2	  "  0.01"
    64	  "  0.25"
   100	  "  0.39"
   500	  "  1.95"
   512	  "  2.00"
  5000	  " 19.53"
 30000	  "117.19"
255997	  "999.99"
256000	  "***.**"
*/
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
	else
	{
		n1 = n1/10; //version 1 = discarding digits below second decimal
	}		
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
/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Range of Grids on LCD: y: 0-159, x: 0-127
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY)
{
	Output_Clear();
	
	char* graphtitle  = title; 
	gb_res = 80;
	
	gb_ymax = (maxY - minY)/gb_res;
	gb_ymin = minY/gb_res;
	gb_xmax = (maxX - minX)/gb_res;
	gb_xmin = minX/gb_res;
	
	if(gb_ymax >127) gb_ymax =127;
	if(gb_xmax >127) gb_xmax =127;
	
	ST7735_DrawString(0, 0, graphtitle, ST7735_YELLOW);
	ST7735_FillRect(0, 32, gb_xmax, gb_ymax, ST7735_Color565(228,228,228));
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
	uint32_t xplot, yplot;
	for(int i=0;i<num;i++)
	{
		x=bufX[i]/gb_res;
		y=bufY[i]/gb_res;
		
		if (gb_ymin < 0)
		{
			yplot = (gb_ymax + gb_ymin) -y +32; //int32_t yplot = (y - gb_ymin)+32;
		}
		else
		{
			yplot = (gb_ymax - gb_ymin) -y +32;
		}
		xplot = (x - gb_xmin);
		
		if(xplot<0) xplot = 0;
		if(xplot>gb_xmax) xplot = gb_xmax;
		if(yplot<32) yplot = 32;
		if(yplot>gb_ymax+32) yplot = gb_ymax+32;
		
		ST7735_DrawPixel(xplot,   yplot,   ST7735_BLUE);
		ST7735_DrawPixel(xplot+1, yplot,   ST7735_BLUE);
		ST7735_DrawPixel(xplot,   yplot+1, ST7735_BLUE);
		ST7735_DrawPixel(xplot+1, yplot+1, ST7735_BLUE);
	}
}

/***********************EXTRA CREDIT***************************/
/*********Extra Credit Functions****************/
/*Test1EC
Calculates floating point in C; used to measure time elapsed for the calculation
Time took to exectue: 2543238*12.5ns = 31.790475 ms
Input: None
Output: None
*/
volatile float T;    // temperature in C
volatile uint32_t N; // 12-bit ADC value
void Test1EC(void){
  for(N=0; N<4096; N++){
    T = 10.0+ 0.009768*N; 	
  }
}

/*Test2EC
Calculates floating point in C; used to measure time elapsed for the calculation
Time took to exectue: 163869*12.5ns = 2.0483625 ms
Input: None
Output: None
*/
volatile uint32_t T2;    // temperature in 0.01 C
volatile uint32_t N;    // 12-bit ADC value
void Test2EC(void){
  for(N=0; N<4096; N++){
    T2 = 1000 + (125*N+64)>>7; 	
  }
}