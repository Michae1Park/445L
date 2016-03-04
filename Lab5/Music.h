// Music.h
// Michael Park, Jack Zhao
// Date Created: 02/24/2016
// Includes notes, struct, and music.c function prototypes
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 02/29/2016

#ifndef _NOTE_H_
#define _NOTE_H_
#include <stdint.h>

#define C4 					(50000000/261)
#define D4 					(50000000/293)
#define E4 					(50000000/330)
#define F4 					(50000000/348)
#define G4 					(50000000/391)
#define A4 					(50000000/440)
#define B4 					(50000000/495)
#define C5 					(50000000/522)
#define D5 					(50000000/587)
#define E5 					(50000000/660)
#define F5 					(50000000/695)
#define G5 					(50000000/782)
#define A5 					(50000000/880)
#define B5 					(50000000/990)
#define C6 					(50000000/1043)
#define Gs4 				(50000000/415)

#define SONG_LENGTH 128


typedef struct {
	uint32_t length;
	uint32_t Song[SONG_LENGTH];
}Music;

//const uint16_t Wave[32];

//unsigned char soundIndex; //varies from 0 to 32

//const uint32_t Song[128];

//static uint16_t stopped;
//static uint16_t altSound;
//int note;

void Music_Init(void);
void PlaySong(void);
void PlaySong2(void);
void addWaves(void);
#endif