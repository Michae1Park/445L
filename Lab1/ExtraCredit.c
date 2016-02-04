
#include <stdio.h>
#include <stdint.h>
// version 1: C floating point
// run with compiler options selected for floating-point hardware
volatile float T;    // temperature in C
volatile uint32_t N; // 12-bit ADC value
void Test1EC(void){
  for(N=0; N<4096; N++){
    T = 10.0+ 0.009768*N; 	
  }
}
/*
// version 2: C fixed-point
volatile uint32_t T2;    // temperature in 0.01 C
volatile uint32_t N;    // 12-bit ADC value
void Test2EC(void){
  for(N=0; N<4096; N++){
    T2 = 1000 + (125*N+64)>>7; 	
  }
}
*/