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
#include "../Shared/tm4c123gh6pm.h"

volatile uint16_t hours, minutes;

//global: hour, min, alarmflag, displayflag, switchflag(for interrupt)

void setTimeInit(void)
{
//disable interrupts
}

//Assuming Time interrupt is disalbed
void setTimeBase(uint32_t choice)
{
    switch(choice) 
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
				//default:
        //incrementHour();  //increment both for debugging
        //incrementMin();
		}

}

void incrementHour(void)
{
  if((++hours)>23) {hours = 0;} //if hour exceeds 23, reset to 0
  else {hours++;}
}

void decrementHour(void)
{
  if((--hours)<0) {hours = 23;} //if hour below 0, reset to 23
  else {hours--;}
}

void incrementMin(void)
{
  if((++minutes)>59) {minutes = 0;} //if min exceeds 59, reset to 0
  else {minutes++;}
}

void decrementMin(void)
{
  if((--minutes)<0) {minutes = 59;} //if min below 0, reset to 59
  else {minutes--;}
}