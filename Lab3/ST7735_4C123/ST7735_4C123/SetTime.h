// SetTime.c
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for functions to set time
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/15/16
#include <stdio.h>
#include <stdint.h>
#include "../Shared/tm4c123gh6pm.h"

//global: hour, min, alarmflag, displayflag, switchflag(for interrupt)

void SetTime_Init(void);
//Assuming Time interrupt is disalbed
void DisplaySetTime(void);

void changeTime(uint32_t buttonSelect);

void incrementHour(void);

void decrementHour(void);

void incrementMin(void);

void decrementMin(void);