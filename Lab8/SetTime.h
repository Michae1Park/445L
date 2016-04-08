// SetTime.h
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for set time function prototypes
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/15/16
#include <stdio.h>
#include <stdint.h>
#include "../Shared/tm4c123gh6pm.h"

enum button_type
{
	DEFAULT,
	INCDEC_TIME_ALARM
};

//global variable for alarm clock
//extern volatile uint16_t time_hours, time_minutes;
//extern volatile uint16_t time_flag;

//void changeTime(void);

//IncrementHour
//Increments global variable time_hours
//Input: None
//Output: None
void incrementHour(void);

//DecrementHour
//Decrements global variable time_hours
//Input: None
//Output: None
void decrementHour(void);

//IncrementMin
//Increments global variable time_minutes
//Input: None
//Output: None
void incrementMin(void);

//DeccrementMin
//Deccrements global variable time_minutes
//Input: None
//Output: None
void decrementMin(void);
