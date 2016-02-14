// SetTime.c
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for functions to set time
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/15/16

//global: hour, min, alarmflag, displayflag, switchflag(for interrupt)
void SetAlarm_Init(void);

void DisplayAlarmTime(void);
//Assuming Time interrupt is disalbed
void changeAlarmTime(uint32_t choice);

void incrementAlarmHour(void);
void decrementAlarmHour(void);
void incrementAlarmMin(void);
void decrementAlarmMin(void);
void DisplayAlarm(void);
