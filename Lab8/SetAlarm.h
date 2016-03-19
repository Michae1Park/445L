// SetTime.c
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for functions to set time
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/15/16

//global: hour, min, alarmflag, displayflag, switchflag(for interrupt)
#include <stdint.h>

extern volatile uint16_t alarm_hours, alarm_minutes;
extern volatile uint16_t alarm_flag;


void setAlarmTimeInit(void);
//Assuming Time interrupt is disalbed
void setAlarmTimeBase(void);

void incrementAlarmHour(void);
void decrementAlarmHour(void);
void incrementAlarmMin(void);

void decrementAlarmMin(void);
void AlarmToggleON(void);
void AlarmToggleOFF(void);
