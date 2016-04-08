// SetTime.c
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for functions to set alarm
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
#include "SetAlarm.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

volatile uint16_t alarm_hours=0;
volatile uint16_t alarm_minutes=0;
volatile uint16_t alarm_flag = 1;

void incrementAlarmHour(void)
{
	long sr = StartCritical();
  if((alarm_hours+1)>23) {alarm_hours = 0;} //if hour exceeds 23, reset to 0
  else {alarm_hours++;}
	EndCritical(sr);
}

void decrementAlarmHour(void)
{
	long sr = StartCritical();
  if((alarm_hours-1)<0) {alarm_hours = 23;} //if hour below 0, reset to 23
  else {alarm_hours--;}
	EndCritical(sr);
}

void incrementAlarmMin(void)
{
	long sr = StartCritical();
  if((alarm_minutes+1)>59) {alarm_minutes = 0;} //if min exceeds 59, reset to 0
  else {alarm_minutes++;}
	EndCritical(sr);
}

void decrementAlarmMin(void)
{
	long sr = StartCritical();
  if((alarm_minutes-1)<0) {alarm_minutes = 59;} //if min below 0, reset to 59
  else {alarm_minutes--;}
	EndCritical(sr);
}
