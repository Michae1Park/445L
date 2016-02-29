#include <stdint.h>
#include "DAC.h"
#include "Music.h"
/*
const uint16_t Wave[32] = {1024,1122,1215,1302,1378,1440,1486,1514,1524,1514,1486,1440,1378,1302,1215,1122,1024,926,833,746,670,608,562,534,524,534,562,608,670,746,833,926};

unsigned char soundIndex = 0; //varies from 0 to 32

const uint32_t Song[128] = {A4, A4, A4, A4, A4, A4, B4, B4, C5, C5, E5, E5, E5, E5, C5, C5, B4, B4, G4, G4, G4, G4, F4, E4, F4, F4, F4, F4, F4, 0, 
	A4, C5, D5, D5, D5, D5, D5, D5, E5, E5, F5, F5, A4, A4, A4, A4, F5, F5, G5, G5, C5, C5, C5, C5, E5, F5, G5, G5, G5, G5, G5, G5, 0, 0,
	F5, F5, F5, F5, F5, F5, E5, E5, D5, D5, A5, A5, A5, A5, F5, F5, E5, E5, C5, C5, C5, C5, B_Flat4, A4, B_Flat4, B_Flat4, B_Flat4, B_Flat4, B_Flat4, 0, 
	D5, F5, G5, G5, G5, G5, G5, G5, A5, A5, B_Flat5, B_Flat5, D5, D5, D5, D5, B_Flat5, B_Flat5, C6, C6, F5, F5, F5, F5, A5, B_Flat5, C6, C6, C6, C6, C6, C6, 0, 0};
static uint16_t stopped = 0;
static uint16_t altSound = 0;
int note = 0;

void PlaySong(void)
{
	DAC_Out(Wave[soundIndex]);
}*/
/*
void PlaySong(void)
{
	if(!stopped)
	{
		soundIndex += 1;
		if(soundIndex > 31)
		{
			soundIndex = 0;
		}
		if(altSound == 0)
		{
			DAC_Out(Wave[soundIndex]);
		}
		else if(altSound == 1)
		{
			DAC_Out(Horn[soundIndex]);
		}
	}
	else
	{
		// Do nothing
	}
}*/