#ifndef _INSTRUMENT_H_
#define _INSTRUMENT_H_
#include <stdint.h>
#define NUM_STEPS 32
typedef struct{
	uint16_t waveForm[NUM_STEPS];
} Instrument;

#endif