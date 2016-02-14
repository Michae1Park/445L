// SetTime.c
// Michael Park, Jack Zhao
// Date Created: 02/10/2016
// class for functions to set time
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/15/16

//global: hour, min, alarmflag, displayflag, switchflag(for interrupt)

void setAlarmTimeInit(void);
//Assuming Time interrupt is disalbed
void setAlarmTimeBase(uint32_t choice);

void incrementAlarmHour(void);
void decrementAlarmHour(void);
void incrementAlarmMin(void);

void decrementAlarmMin(void);
void AlarmToggleON(void);
void AlarmToggleOFF(void);