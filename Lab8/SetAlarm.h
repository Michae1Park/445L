// SetAlarm.h
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for functions to set time
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 4/8/16

//global: hour, min, alarmflag, displayflag, switchflag(for interrupt)
#include <stdint.h>

//global variable for alarm clock
extern volatile uint16_t alarm_hours, alarm_minutes;
extern volatile uint16_t alarm_flag;

//IncrementAlarmHour
//Increments global variable alarm_hours
//Input: None
//Output: None
void incrementAlarmHour(void);

//DecrementAlarmHour
//Decrements global variable alarm_hours
//Input: None
//Output: None
void decrementAlarmHour(void);

//IncrementAlarmMin
//Increments global variable alarm_minutes
//Input: None
//Output: None
void incrementAlarmMin(void);

//DeccrementAlarmMin
//Deccrements global variable alarm_minutes
//Input: None
//Output: None
void decrementAlarmMin(void);
