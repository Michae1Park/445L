
void PeriodMeasure_Init(void);
uint32_t returnPeriod(void);
void integralControl(void);
void Timer2_Init(void(*task)(void), unsigned long period);
