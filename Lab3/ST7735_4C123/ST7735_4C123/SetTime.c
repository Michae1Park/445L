// SetTime.c
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for functions to set time
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/15/16

#include <stdio.h>
#include <stdint.h>
#include "SetTime.h"
#include "Timer1.h"
#include "TimeDisplay.h"
#include "../Shared/tm4c123gh6pm.h"

extern long StartCritical (void);    // previous I bit, disable interrupts
extern void EndCritical(long sr);    // restore I bit to previous value
extern volatile uint16_t Time_Seconds, Time_Minutes, Time_Hours; //from Main Class
extern volatile uint32_t Display_Mode; //from TimeDisplay Class

void SetTime_Init(void)
{

}
void DisplaySetTime(void){
	//disable interrupts
	long critical=StartCritical();
	Display_Mode=1;
	DisplayRefresh();
	EndCritical(critical);
}
//Assuming Time interrupt is disalbed
void changeTime(uint32_t buttonSelect)
{
		//enable critical section
		long critical=StartCritical();
	
		//set time seconds to 0
		Time_Seconds=0;
	
    switch(buttonSelect) 
    {
      case 0:
        incrementHour();
        break;
      case 1:
        decrementHour();
        break;
      case 2:
        incrementMin();
        break;
      case 3:
        decrementMin();
        break;
		}
		EndCritical(critical);
}

void incrementHour(void)
{
  if((Time_Hours+1)>23) {Time_Hours = 0;} //if hour exceeds 23, reset to 0
  else {Time_Hours++;}
}

void decrementHour(void)
{
  if((Time_Hours-1)<0) {Time_Hours = 23;} //if hour below 0, reset to 23
  else {Time_Hours--;}
}

void incrementMin(void)
{
  if((Time_Minutes+1)>59) {Time_Minutes = 0;} //if min exceeds 59, reset to 0
  else {Time_Minutes++;}
}

void decrementMin(void)
{
  if((Time_Minutes-1)<0) {Time_Minutes = 59;} //if min below 0, reset to 59
  else {Time_Minutes--;}
}

