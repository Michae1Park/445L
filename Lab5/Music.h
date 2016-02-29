#ifndef _NOTE_H_
#define _NOTE_H_
#include <stdint.h>

#define C4 					(50000000/261)
#define C_Sharp4		(50000000/440)
#define D4 					(50000000/293)
#define E4 					(50000000/330)
#define F4 					(50000000/348)
#define F_Sharp4		(50000000/371)
#define G4 					(50000000/391)
#define A4 					(50000000/440)
#define B_Flat4			(50000000/464)
#define B4 					(50000000/495)
#define C5 					(50000000/522)
#define C_Sharp5		(50000000/557)
#define D5 					(50000000/587)
#define E5 					(50000000/660)
#define F5 					(50000000/695)
#define F_Sharp5		(50000000/742)
#define G5 					(50000000/782)
#define A5 					(50000000/880)
#define B_Flat5			(50000000/927)
#define B5 					(50000000/990)
#define C6 					(50000000/1043)

#define SONG_LENGTH 32

/*
typedef struct {
	uint32_t periodCycles;
	uint8_t duration12thnotes;
	uint8_t dynamicPercent;
	uint16_t waveForm[SONG_LENGTH];
}Note;

const uint16_t Wave[32];

unsigned char soundIndex; //varies from 0 to 32

const uint32_t Song[128];

static uint16_t stopped;
static uint16_t altSound;
int note;
*/
void PlaySong(void);
#endif