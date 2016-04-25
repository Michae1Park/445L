// PeriodMeasure.h
// Michael Park, Jack Zhao, Corey Cormier
// April 20, 2016

#include <stdint.h>

extern volatile int32_t currentDuty;
extern volatile int32_t newDuty;

void PeriodMeasure_Init(void);
uint32_t returnPeriod(void);
void integralControl(void);
void Timer2_Init(void(*task)(void), unsigned long period);
