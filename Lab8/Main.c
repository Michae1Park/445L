// AlarmClockMain.c 
// Michael Park, Jack Zhao
// Date Created: 2/12/2016
// Driver for Alarm clock. Includes interrupts used
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 2/15/2016

#include <stdio.h>
#include <stdint.h>
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

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define PERIOD					0x04C4B400
#define Tensec					0x2FAF0800	//number of cycle for 10s when each cycle = 12.5ns
#define A_440 					181818

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
extern void Output_Init2();

char FetchStock[] = "GET /webservice/v1/symbols/GOOG/quote?format=json HTTP/1.1\r\nHost:finance.yahoo.com\r\nConnection: keep-alive\r\n\r\n";
char FetchTemp[] = "GET /v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text%3D%22nome%2C%20ak%22)&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys HTTP/1.1\r\nHost:query.yahooapis.com\r\nConnection: keep-alive\r\n\r\n";
//volatile uint16_t Mode;
volatile uint16_t active_In10s = 1;
volatile uint32_t counts;
volatile uint32_t timeout=0;
//volatile uint16_t toggleSound=1;
volatile uint16_t AllowAlarmChange=1;
volatile char temp[3];


int main(void)
{
	DisableInterrupts();
//system component setup
	PLL_Init(Bus80MHz);                 // set system clock to 80 MHz
	LED_Init();
  Output_Init();       // UART0 only used for debugging
  printf("\n\r-----------\n\rSystem starting...\n\r");
	Timer1_Init(0, PERIOD);							// Init Timer1 for global clock
  ST7735_InitR(INITR_REDTAB);					// Init PORTA and LCD initializations
	ADC0_InitSWTriggerSeq3_Ch9(); 
	Switch_Init(); 											// Init PORTB and switch initialization
	Alarm_Init();												//togglesound flag triggers alarm
	Timer0A_Init(0,A_440);
//SysTick_Init(80000);
//PortF_Init();
	ST7735_DrawString(0, 2, "\n\r-----------\n\rSystem starting...\n\r", ST7735_WHITE);
  ESP8266_Init(115200);      // global enable interrupts
	ESP8266_GetVersionNumber();
	
	toggleSound = 1;
//	display_status = PG1;
	EnableInterrupts();							// Enable Interrupts
	
//USER FUNCTION INIT							// Self Described Init Functions									
	Display_PG1();
//Display_PG2();
//Display_PG3();
//Display_PG4();

//alarm snooze init
//	toggleSound = 1;
	display_status = PG1;


	while(1){
    ESP8266_GetStatus();
		
    if(ESP8266_MakeTCPConnection("query.yahooapis.com")){ // open socket in YAHOO stock server  finance.yahoo.com
      printf("TCPConnection established\n\r");
			
			LED_GreenOn();
      ESP8266_SendTCP(FetchTemp);

    }
		
    ESP8266_CloseTCPConnection();
//		EnableInterrupts();							// Enable Interrupts
    while(Board_Input()==0){
      WaitForInterrupt();
    }; // wait for touch
    LED_GreenOff();
    LED_RedToggle();
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



/*
void AllowAlarmChangeMode(void){
	AllowAlarmChange=1;
}
void DisAllowAlarmChangeMode(void){
		AllowAlarmChange=0;
}*/

/*
// this is used for printf to output to the usb uart
int fputc(int ch, FILE *f){
  UART_OutChar(ch);
  return 1;
}*/

#ifdef __TI_COMPILER_VERSION__
  //Code Composer Studio Code
#include "file.h"
int uart_open(const char *path, unsigned flags, int llv_fd){
  UART_Init();
  return 0;
}
int uart_close( int dev_fd){
  return 0;
}
int uart_read(int dev_fd, char *buf, unsigned count){char ch;
  ch = UART_InChar();    // receive from keyboard
  ch = *buf;         // return by reference
  UART_OutChar(ch);  // echo
  return 1;
}
int uart_write(int dev_fd, const char *buf, unsigned count){ unsigned int num=count;
  while(num){
    UART_OutChar(*buf);
    buf++;
    num--;
  }
  return count;
}
off_t uart_lseek(int dev_fd, off_t ioffset, int origin){
  return 0;
}
int uart_unlink(const char * path){
  return 0;
}
int uart_rename(const char *old_name, const char *new_name){
  return 0;
}

/*
//------------Output_Init------------
// Initialize the UART for 115,200 baud rate (assuming 3 MHz bus clock),
// 8 bit word length, no parity bits, one stop bit
// Input: none
// Output: none
void Output_Init(void){int ret_val; FILE *fptr;
  UART_Init();
  ret_val = add_device("uart", _SSA, uart_open, uart_close, uart_read, uart_write, uart_lseek, uart_unlink, uart_rename);
  if(ret_val) return; // error
  fptr = fopen("uart","w");
  if(fptr == 0) return; // error
  freopen("uart:", "w", stdout); // redirect stdout to uart
  setvbuf(stdout, NULL, _IONBF, 0); // turn off buffering for stdout

}
#else
//Keil uVision Code
//------------Output_Init------------
// Initialize the Nokia5110
// Input: none
// Output: none
//------------Output_Init------------
// Initialize the UART for 115,200 baud rate (assuming 16 MHz bus clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void Output_Init(void){
  UART_Init();
}
*/
#endif

