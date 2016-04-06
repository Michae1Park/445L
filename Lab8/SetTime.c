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
#include "Display.h"
#include "../Shared/tm4c123gh6pm.h"
#include "Common.h"
#include "ST7735.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value


void incrementHour(void)
{
	long sr = StartCritical();
  if((Time_Hours+1)>23) {Time_Hours = 0;} //if hour exceeds 23, reset to 0
  else {Time_Hours++;}
	Time_Seconds = 0;
	EndCritical(sr);
}

void decrementHour(void)
{
	long sr = StartCritical();
  if((Time_Hours-1)<0) {Time_Hours = 23;} //if hour below 0, reset to 23
  else {Time_Hours--;}
	Time_Seconds = 0;
	EndCritical(sr);
}

void incrementMin(void)
{
	long sr = StartCritical();
  if((Time_Minutes+1)>59) {Time_Minutes = 0;} //if min exceeds 59, reset to 0
  else {Time_Minutes++;}
	Time_Seconds = 0;
	EndCritical(sr);
}

void decrementMin(void)
{
	long sr = StartCritical();
  if((Time_Minutes-1)<0) {Time_Minutes = 59;} //if min below 0, reset to 59
  else {Time_Minutes--;}
	Time_Seconds = 0;
	EndCritical(sr);
}

