// AlarmClockMain.c 
// Michael Park, Jack Zhao
// Date Created: 2/12/2016
// Driver for Alarm clock. Includes interrupts used
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 2/15/2016

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ST7735.h"
#include "Display.h"
#include "SetTime.h"
#include "PLL.h"
#include "Timer1.h"
#include "Switch.h"
#include "SetAlarm.h"
#include "../Shared/tm4c123gh6pm.h"
#include "PWM.h"
#include "SysTickInts.h"
#include "Common.h"
#include "Timer0A.h"
#include "ADCSWTrigger.h"
#include "esp8266.h"
#include "LED.h"
#include "UART.h"
#include "Timer2.h"
#include "Timer1.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define PERIOD					0x04C4B400
#define Tensec					0x2FAF0800	//number of cycle for 10s when each cycle = 12.5ns
#define A_440 					181818
#define BUFFER_SIZE 1024

#define DEBUG	1	//commnet out this line to disable debugging feature for measuring adc jitter

enum ModeSetting
{  
	SetTime_Mode, 
	SetAlarm_Mode, 
	ToggleAlarm_Mode, 
	TimeDisplay_Mode
};  

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void MainMenu(void);
void AllowAlarmChangeMode(void);
void DisAllowAlarmChangeMode(void);
void PortF_Init(void);
void Alarm_Init(void);
void Change_Display(void);
void parseWeatherBuff(void);
void parseGOOGBuff(void);
void parseStockBuff(void);
void parseRedditBuff(void);

extern void Output_Init2();

char FetchStockGOOG[] = "GET /webservice/v1/symbols/GOOG/quote?format=json HTTP/1.1\r\nHost:finance.yahoo.com\r\nConnection: keep-alive\r\n\r\n";
char FetchStock[] = "GET /webservice/v1/symbols/AAPL,FB/quote?format=json HTTP/1.1\r\nHost:finance.yahoo.com\r\nConnection: keep-alive\r\n\r\n";
char FetchTemp[] = "GET /v1/public/yql?q=select+item.condition+from+weather.forecast+where+woeid+%3D+2357536&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys HTTP/1.1\r\nHost:query.yahooapis.com\r\nConnection: keep-alive\r\n\r\n";
char FetchTrends[] ="GET /r/Showerthoughts/top/.json?limit=1 HTTP/1.1\r\nHost:www.reddit.com\r\nUser-Agent: ESP8266_HTTP_Client\r\nConnection: keep-alive\r\n\r\n";
//volatile uint16_t Mode;
volatile uint16_t active_In10s = 1;
volatile uint32_t counts;
volatile uint32_t timeout=0;
//volatile uint16_t toggleSound=1;
volatile uint16_t AllowAlarmChange=1;

volatile uint32_t ADCvalue;
volatile uint32_t prevADCvalue;
int pg=0;
int prevpg=0;
int pulltrigger=1;




int main(void)
{
	DisableInterrupts();
//system component setup
	PLL_Init(Bus80MHz);                 // set system clock to 80 MHz
	LED_Init();
  Output_Init();       // UART0 only used for debugging
	ST7735_InitR(INITR_REDTAB);					// Init PORTA and LCD initializations
	Output_Clear();		
	ST7735_DrawString(0, 1, "System starting...\n\r", ST7735_WHITE);
	ESP8266_Init(115200);      // global enable interrupts
	ESP8266_GetVersionNumber();
	Output_Clear();	
	ST7735_DrawString(0, 1, "ESP initialized...\n\r", ST7735_WHITE);
	Output_Clear();	
	
	for(int i=0;i<1000;i++){
		int delay=0;
	}
	
	Timer1_Init(0, PERIOD);							// Init Timer1 for global clock
	Timer2_Init(&Change_Display, 4000000);

	ADC0_InitSWTriggerSeq3_Ch9(); 
	Switch_Init(); 											// Init PORTB and switch initialization
	Alarm_Init();												//togglesound flag triggers alarm
	Timer0A_Init(0,A_440);
//SysTick_Init(80000);
//PortF_Init();

	
	toggleSound = 1;
//	display_status = PG1;
	EnableInterrupts();							// Enable Interrupts
	
//USER FUNCTION INIT							// Self Described Init Functions									
//	Display_PG1();
//Display_PG2();
//Display_PG3();
//Display_PG4();

//alarm snooze init
//	toggleSound = 1;
	//display_status = PG1;
	while(1){
		for(int i =0;i<1000;i++){
		}

		if(pulltrigger==1){
    ESP8266_GetStatus();
		//ST7735_DrawString(0, 1, "Start loop", ST7735_WHITE);
    if(ESP8266_MakeTCPConnection("query.yahooapis.com")){ // open socket in YAHOO stock server  finance.yahoo.com
			//ST7735_DrawString(0, 1, "TCPloop", ST7735_WHITE);
      //printf("TCPConnection established\n\r");
      ESP8266_SendTCP(FetchTemp,0);
			ESP8266_CloseTCPConnection();
			//while(1){};
    }
		parseWeatherBuff();

		if(ESP8266_MakeTCPConnection("finance.yahoo.com")){ // open socket in YAHOO stock server  finance.yahoo.com
      printf("TCPConnection established\n\r");
      ESP8266_SendTCP(FetchStockGOOG,1);
			ESP8266_CloseTCPConnection();
    }	
		parseGOOGBuff();
		
		
		if(ESP8266_MakeTCPConnection("finance.yahoo.com")){ // open socket in YAHOO stock server  finance.yahoo.com
      printf("TCPConnection established\n\r");
      ESP8266_SendTCP(FetchStock,2);
			ESP8266_CloseTCPConnection();
    }
		parseStockBuff();
		if(ESP8266_MakeTCPConnection("www.reddit.com")){ // open socket in YAHOO stock server  finance.yahoo.com
			//ST7735_DrawString(0, 2, "TCP established", ST7735_WHITE);
      printf("TCPConnection established\n\r");
      ESP8266_SendTCP(FetchTrends,3);
			ESP8266_CloseTCPConnection();
		}
		parseRedditBuff();
	}
		/*
		LED_GreenToggle();
    LED_GreenOff();
    LED_RedToggle();*/
		if(Time_Minutes==59 ||Time_Minutes==58){
			pulltrigger=1;
		}
		else{
			pulltrigger=0;
		}
  }

}


/*
Alarm Init
*/
void Alarm_Init(void)
{
	//PORTD INIT (For Alarm Sound)
	volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x00000010;  // 1) activate clock for Port E
  delay = SYSCTL_RCGCGPIO_R;        // allow time for clock to start
  
	GPIO_PORTE_CR_R |= 0x02;           // allow changes to PE1
	GPIO_PORTE_DIR_R |= 0x02;        // make PE1 op
  GPIO_PORTE_AFSEL_R &= ~0x02;     // disable alt funct on PE1
  GPIO_PORTE_DEN_R |= 0x02;        // enable digital I/O on PE1
                                   // configure PE1 as GPIO
  GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFFFFF0F);
  GPIO_PORTE_AMSEL_R &= ~0x02;
}

/*
PORTF Init
*/
void PortF_Init(void)
{
	//PORTF INIT (For Heart Beat)
  SYSCTL_RCGCGPIO_R |= 0x20;  // activate port F
  GPIO_PORTF_DIR_R |= 0x06;   // make PF2 and PF1 output (PF2 built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;// disable alt funct on PF2 and PF1
  GPIO_PORTF_DEN_R |= 0x06;   // enable digital I/O on PF2 and PF1
                              // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;     // disable analog functionality on PF
}

void Change_Display(void){
	ADCvalue = ADC0_InSeq3();

		if((ADCvalue<1024)) 
		{
			pg=1;
		}
		else if((ADCvalue>=1024) && (ADCvalue<2048)) 
		{
			pg=2;
		}
		else if((ADCvalue>=2048) && (ADCvalue<3072))
		{
			pg=3;
		}
		else 
		{
			pg=4;
		}
		
		if(pg==1 && pg!=prevpg){
			Output_Clear();		
			
			Display_PG1();
			//ST7735_DrawString(0, 6, dayString, ST7735_WHITE);
			//ST7735_DrawString(0, 7, monthString, ST7735_WHITE);
		}
		else if(pg==2 && pg!=prevpg){
			Output_Clear();		
			Display_PG2();
		}
		else if(pg==3 &&pg!=prevpg){
			Output_Clear();		
			Display_PG3();
		}
		else if(pg==4 && pg!= prevpg){
			Output_Clear();		
			Display_PG4();
		}
		else{
			
		}
		prevpg=pg;
}
void parseWeatherBuff(void){
	printf("entered loop");
	ST7735_DrawString(0, 15, "parse started", ST7735_WHITE);
	char* tempPointer= strstr(weatherBuff, "temp");
	if(tempPointer != NULL){
		strncpy(temp, tempPointer+7,2);
		temp[2]=0;
		//ST7735_DrawString(0, 3, temp, ST7735_WHITE);
	}
	char* textPointer=strstr(weatherBuff,"text");
	//ST7735_DrawString(0, 3, textPointer, ST7735_WHITE);
	if(textPointer != NULL){
		int size=0;
		textPointer+=7;
		while(strncmp(textPointer+size,"}",1) != 0){
			size++;
		}
		//memset(&description, 0,sizeof(description));
		strncpy(description,textPointer,size-1);
		description[size]=0;
	}
	ST7735_DrawString(0, 15, description, ST7735_WHITE);
	char monthC[2];
	char dayC[2];
	char* dayPointer =strstr(weatherBuff,"created");
	if(dayPointer != NULL){
		dayPointer+=18;
		//memset(&dayC, 0,sizeof(dayC));
		strncpy(dayC, dayPointer,2);
	}
	char* monthPointer =strstr(weatherBuff,"created");
	if(monthPointer != NULL){
		monthPointer+=15;
		//memset(&monthC, 0,sizeof(monthC));
		strncpy(monthC, monthPointer,2);
	}	
	sscanf(monthC, "%d", &month);
	sscanf(dayC, "%d", &day);

	char*	dayStringPointer =strstr(weatherBuff,"date");
	if(dayStringPointer != NULL){
		dayStringPointer+=7;
		//memset(&dayString, 0,sizeof(dayString));
		strncpy(dayString, dayStringPointer,3);
		dayString[3]=0;
	}
	
	char * monthStringPointer =strstr(weatherBuff,"date");
	if(monthStringPointer != NULL){
		
		monthStringPointer+=15;
		//memset(&monthString, 0,sizeof(monthString));
		strncpy(monthString, monthStringPointer,3);
		monthString[3]=0;
	}
}
void parseGOOGBuff(void){
	
ST7735_DrawString(0,15, "start goog",ST7735_WHITE);
	char* stockGOOGPointer= strstr(stockGOOGBuff, "Alphabet Inc.");
	if(stockGOOGPointer != NULL){
		stockGOOGPointer+=27;
		//memset(&GOOGquote, 0,sizeof(GOOGquote));
		strncpy(GOOGquote, stockGOOGPointer,6);
		GOOGquote[6]=0;
			ST7735_DrawString(0, 15, GOOGquote, ST7735_WHITE);
	}
}
void parseStockBuff(void){
	ST7735_DrawString(0, 15, "Start parse stock OTHERS", ST7735_WHITE);
	char* stockAAPLPointer=strstr(stockRestBuff, "Apple Inc.");
	if(stockAAPLPointer != NULL){
		stockAAPLPointer+=24;
		//memset(&AAPLquote, 0,sizeof(AAPLquote));
		strncpy(AAPLquote, stockAAPLPointer,6);
		AAPLquote[6]=0;
	}
	ST7735_DrawString(0, 15, stockAAPLPointer+10, ST7735_WHITE);
	char* stockFBPointer=strstr(stockRestBuff+5, "Facebook, Inc.");
	if(stockFBPointer != NULL){
		stockFBPointer+=28;
		//memset(&FBquote, 0,sizeof(FBquote));
		strncpy(FBquote, stockFBPointer,6);
		FBquote[6]=0;
	}
	ST7735_DrawString(0, 15, stockFBPointer+10, ST7735_WHITE);
}
void parseRedditBuff(void){
	
	ST7735_DrawString(0, 15, "Parsed", ST7735_WHITE);
	char* redditPointer= strstr(redditBuff, "title");
	if(redditPointer != NULL){
		redditPointer+=9;
		int size=0;
		while(strncmp(redditPointer+size,":",1) != 0){
			size++;
		}
		strncpy(redditTitle, redditPointer,size-17);
		redditTitle[size-16]=0;
		redditSize=size;
		ST7735_DrawString(0, 15, redditTitle, ST7735_WHITE);
	}
}
