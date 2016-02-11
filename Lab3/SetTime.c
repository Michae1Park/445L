// SetTime.c
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for functions to set time
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/15/16

#include <stdio.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"

//global: hour, min, alarmflag, displayflag, switchflag(for interrupt)

ISRSwitch(PB0 || PB1 || PB2 || PB3)
{
  if (PBs actually on)	//checking for debouncing
  {
    switchflag = 1;
  }
}

//Assuming Time interrupt is disalbed
void setTime(void)
{
  uint32_t switchbuff;

  if (switchflag)	//switch detected
  {
  	switchflag = 0;
    switchbuff = PB;
    switchbuff &= 0x000F; //mask for PB0-3

    switch(switchtmp) 
    {
      case PB0:
        incrementHour();
        break;
      case PB1:
        decrementHour();
        break;
      case PB2:
        incrementMin();
        break;
      case PB3:
        decrementMin();
        break;
      default:
        incrementHour();  //increment both for debugging
        incrementMin();
    }
  }
}

void incrementHour(void)
{
  if((++hour)>23) {hour = 0;} //if hour exceeds 23, reset to 0
  else {hour++;}
}

void decrementHour(void)
{
  if((--hour)<0) {hour = 23;} //if hour below 0, reset to 23
  else {hour--;}
}

void incrementMin(void)
{
  if((++min)>59) {min = 0;} //if min exceeds 59, reset to 0
  else {min++;}
}

void decrementMin(void)
{
  if((--min)<0) {min = 59;} //if min below 0, reset to 59
  else {min--;}
}
