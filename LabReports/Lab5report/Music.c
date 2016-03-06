// Music.c
// Michael Park, Jack Zhao
// Date Created: 02/24/2016
// Includes music data and timer invoked task functions
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/29/2016

#include <stdint.h>
#include "DAC.h"
#include "Music.h"


#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define LEDS      (*((volatile uint32_t *)0x40025038))
	
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
	

const uint16_t Wave[32] = {1024,1124,1218,1304,1380,1443,1488,1516,1526,1516,1488,1442,1380,1304,1218,1124,1026,928,835,748,672,610,564,536,526,536,564,610,672,748,835,928};
const uint16_t Horn[32] = {1065,1084,1122,1277,1680,1750,1277,757,663,663,705,734,770,847,1040,1136,1211,1333,1467,1547,1429,1589,1372,1088,709,520,450,492,568,685,803,994};


const uint32_t Song3[128] ={0, 0, A4, A4, E5, E5, A4, A4, 
E5, E5, E5, D5, D5, D5, D5, D5, 
D5, D5, B4, B4, D5, D5, D5, B4, 
A4, A4, A4, A4, A4, A4, A4, A4, 
A4, A4, E4, E4, E4, E4, A4, A4, 
A4, A4, A4, B4, B4, B4, B4, B4,
B4, B4, B4, B4, D5, D5, B4, B4,
A4, A4, A4, A4, A4, A4, A4, A4,

A4, A4, A4, A4, A4, A4, A4, A4,
E5, E5, E5, E5, E5, E5, E5, E5,
E5, D5, D5, A4, B4, A4, G4, E4,
D4, D4, D4, D4, D4, D4, D4, D4,
D4, D4, D4, D4, D4, D4, D4, C4,
C4, C4, C4, C4, C4, C4, C4, C4,
0, 0, 0, 0, 0, 0, 0, 0, };
	
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
volatile uint32_t addedWaves;
volatile uint32_t addedWaves2;
volatile uint32_t refresh =0;

void Music_Init(void){
	current_song.length=128;
	for(int i =0; i <current_song.length; i++){
		current_song.Song[i]=Song3[i];
	}
}
	

void PlaySong(void)
{

	if(!stop)
	{
			refresh++;
		PF1 ^= 0x02;
		soundIndex += 1;
		if(soundIndex > 31)
		{
			soundIndex = 0;
		}
		if(changeSound == 0)
		{
			//DAC_Out(Wave[soundIndex]);//
		}
		else if(changeSound==1){
			DAC_Out(Horn[soundIndex]);
		}
		PF1 ^= 0x02;
	}
}

void PlaySong2(void)
{
	//refresh+=1;
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
			//DAC_Out(Wave[soundIndex2]);
		}
		else if(changeSound==1){
			DAC_Out(Horn[soundIndex2]);
		}
		PF1 ^= 0x02;
	}
}


void addWaves(void){
long sr=StartCritical();

	addedWaves=Wave[soundIndex]+Wave[soundIndex2];//
	addedWaves=(addedWaves)/2;//
//	if(addedWaves<1024)
//	{
//		addedWaves+=refresh;
//	}
//	else{
//		addedWaves-=refresh;
//	}
	addedWaves2=Horn[soundIndex]+Horn[soundIndex2];
	addedWaves2=addedWaves2/2;
	
	
	if(changeSound == 0)
	{
			DAC_Out(addedWaves);
	}
	else if(changeSound==1){
			DAC_Out(addedWaves2);
	}
	EndCritical(sr);
}