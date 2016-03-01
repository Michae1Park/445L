#include <stdint.h>
#include "DAC.h"
#include "Music.h"


#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define LEDS      (*((volatile uint32_t *)0x40025038))
	

const uint16_t Wave[32] = {1024,1122,1215,1302,1378,1440,1486,1514,1524,1514,1486,1440,1378,1302,1215,1122,1024,926,833,746,670,608,562,534,524,534,562,608,670,746,833,926};
const uint16_t Horn[32] = {1063,1082,1119,1275,1678,1748,1275,755,661,661,703,731,769,845,1039,1134,1209,1332,1465,1545,1427,1588,1370,1086,708,519,448,490,566,684,802,992};


const uint32_t Song[128] = {A4, A4, A4, A4, A4, A4, B4, B4, C5, C5, E5, E5, E5, E5, C5, C5, B4, B4, G4, G4, G4, G4, F4, E4, F4, F4, F4, F4, F4, 0, 
	A4, C5, D5, D5, D5, D5, D5, D5, E5, E5, F5, F5, A4, A4, A4, A4, F5, F5, G5, G5, C5, C5, C5, C5, E5, F5, G5, G5, G5, G5, G5, G5, 0, 0,
	F5, F5, F5, F5, F5, F5, E5, E5, D5, D5, A5, A5, A5, A5, F5, F5, E5, E5, C5, C5, C5, C5, B_Flat4, A4, B_Flat4, B_Flat4, B_Flat4, B_Flat4, B_Flat4, 0, 
	D5, F5, G5, G5, G5, G5, G5, G5, A5, A5, B_Flat5, B_Flat5, D5, D5, D5, D5, B_Flat5, B_Flat5, C6, C6, F5, F5, F5, F5, A5, B_Flat5, C6, C6, C6, C6, C6, C6, 0, 0};
	
const uint32_t Song2[128]={0, 0, E5, E5, A5, A5, E5, E5, 
G5, G5, G5, F5, F5, F5, F5, F5, 
F5, F5, D5, D5, G5, G5, G5, D5, 
E5, E5, E5, E5, E5, E5, E5, E5, 
E5, E5, A4, A4, A4, A4, C5, C5, 
E5, E5, E5, D5, D5, D5, D5, D5,
D5, D5, D5, D5, G5, G5, D5, D5,
E5, E5, E5, E5, E5, E5, E5, E5,
E5, E5, E5, E5, E5, E5, E5, E5,
A5, A5, A5, A5, A5, A5, A5, A5,
A5, G5, F5, E5, D5, C5, B4, A4,
Gs4, Gs4, Gs4, Gs4, Gs4, Gs4, Gs4, Gs4,
Gs4, Gs4, F4, F4, F4, F4, F4, E4,
E4, E4, E4, E4, E4, E4, E4, E4,
0, 0, 0, 0, 0, 0, 0, 0, };


volatile uint32_t note = 0;
volatile Music current_song;
unsigned char soundIndex = 0; //varies from 0 to 32
unsigned char soundIndex2 = 0; //varies from 0 to 32
extern volatile uint16_t stop;
extern volatile uint16_t changeSound;

void Music_Init(void){
	current_song.length=128;
	for(int i =0; i <current_song.length; i++){
		current_song.Song[i]=Song[i];
	}
}
	

void PlaySong(void)
{
	if(!stop)
	{
		PF1 ^= 0x02;
		soundIndex += 1;
		if(soundIndex > 31)
		{
			soundIndex = 0;
		}
		if(changeSound == 0)
		{
			DAC_Out(Wave[soundIndex]);
		}
		else if(changeSound==1){
			DAC_Out(Horn[soundIndex]);
		}
		PF1 ^= 0x02;
	}
}

void PlaySong2(void)
{
	if(!stop)
	{
		PF1 ^= 0x02;
		soundIndex2 += 1;
		if(soundIndex2 > 31)
		{
			soundIndex2 = 0;
		}
		if(changeSound == 0)
		{
			DAC_Out(Wave[soundIndex2]);
		}
		else if(changeSound==1){
			DAC_Out(Horn[soundIndex2]);
		}
		PF1 ^= 0x02;
	}
}
