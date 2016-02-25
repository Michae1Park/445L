#ifndef _NOTE_H_
#define _NOTE_H_
#include <stdint.h>
#define C2 38167
#define Dflat2 36025
#define D2 34003
#define Eflat2 32095
#define E2 30293
#define F2 28593
#define Gflat2 26988
#define G2 25474
#define Aflat2 24044
#define A2 22694
#define Bflat2 21421
#define B2 20218
#define C3 19083
#define Dflat3 18012
#define D3 17001
#define Eflat3 16047
#define E3 15146
#define F3 14296
#define Gflat3 13494
#define G3 12737
#define Aflat3 12022
#define A3 11347
#define Bflat3 10710
#define B3 10109
#define C4 9541
#define Dflat4 9006
#define D4 8500
#define Eflat4 8023
#define E4 7573
#define F4 7148
#define Gflat4 6747
#define G4 6368
#define Aflat4 6011
#define A4 5673
#define Bflat4 5355
#define B4 5054
#define C5 4770
#define Dflat5 4503
#define D5 4250
#define Eflat5 4011
#define E5 3786
#define F5 3574
#define Gflat5 3373
#define G5 3184
#define Aflat5 3005
#define A5 2836
#define Bflat5 2677
#define B5 2527
#define C6 2385
#define Dflat6 2251
#define D6 2125
#define Eflat6 2005
#define E6 1893
#define F6 1787
#define Gflat6 1686
#define G6 1592
#define Aflat6 1502
#define A6 1418
#define Bflat6 1338
#define B6 1263

#define SONG_LENGTH 100

typedef struct {
	uint32_t periodCycles;
	uint8_t duration12thnotes;
	uint8_t dynamicPercent;
	uint16_t waveForm[SONG_LENGTH];
}Note;
#endif