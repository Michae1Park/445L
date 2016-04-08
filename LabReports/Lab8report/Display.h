// Display.h
// Michael Park, Jack Zhao
// Date Created: 2/12/2016
// Includes prototypes of analog and digital time display functions
// Lab Number: 16340
// TA: Mahesh Srinivasan
// Last Revised: 4/6/2016


#include <stdint.h>

#define ACTUALDATE 20

enum display_stat 
{
	PG1,
	PG2,
	PG3, 
	PG4
};

extern volatile uint16_t display_mode;
extern char weather_temp[3];
extern char weather_weather[6];
extern volatile uint16_t display_status;

//Display
//Builtint Clock
//Builtin Alarm time
//Pulled Weather temperature
//							 Rainy, Cloudy, Sunny, etc
//
//Recommended Outfit
//						100+ : Topless
//						80+	 : Shorts 		 + 	Short Sleeves
//						75+	 : Shorts 		 + 	Long Sleeves
//						60+	 : Pants/Jeans + 	Short Sleeves (optional jacket)
//						50+	 : Pants/Jeans +	Long Sleeves
//						35+	 : Pants/Jeans +  Long Sleeves + Jacket/Hoodie
//						35-  : Coat
//SW1	: SetTime
//SW2 	: SetAlarm
//SW3	: Snooze
//SW4	: Update Weather Data, Recommneded Outfit will follow
//System Automatically updates at Midnight, Else update switch needs to be pressed to update
void Display_PG1(void);

//Display Calendar
//Display Month and Day In Standard Calendar Grid
//Grid for the Day will be marked red, else black
//Input: SlidePot
void Display_PG2(void);

//Display important news etc
//Input: SlidePot, Update SW4
void Display_PG3(void);

//Display Stock, Currency Exchange rate, Gas Price
//Input: Slidepot, Update SW4
void Display_PG4(void);

// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n);

//Sunny, Rainy, Cloudy, Stormy, Snowy, 
//const char Weather_DB1 [5] = {'S', 'R', 'C', 'S', 'S'};
//const char Weather_DB2 [5] = {'u', 'a', 'l', 't', 'n'};
	
	