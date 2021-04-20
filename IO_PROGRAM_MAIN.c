//DETAILS:
//Written by Weston Forbes.
//westonforbes@automaticspring.com, westonforbes@gmail.com

//NOTE ON NAMING CONVENTION:
//Each method should start with the name of the file its contained in. For example, every method in IO_DEV_DRAW should begin with DRAW.
//This makes it easier to manually locate where methods are in the project structure.

//BASIC DESCRIPTION:
//This file handles main loop operations.

//Includes.
#include <time.h>
#include <sys/time.h>
#include "IO_PROGRAM_DRAW.c"
#include "IO_PROGRAM_WIRE.c"

#define SLEEP_TIME 10

//Method prototypes.
/// <summary>
/// This is the meat and potatoes function.
/// </summary>
int MAIN_LOOP(void);

/// <summary>
/// This method will sleep the thread for a certain period of time.
/// </summary>
static void SLEEP_MS(long MILLISECONDS);

/// <summary>
/// This method returns the time in a standard tm struct.
/// </summary>
struct tm READ_TIME(void);

//Method implementations.
int main(){
	//Setup a variable to hold return codes from called functions.
	int RETURN_CODE = 0;

	WIRE_SETUP();

	//Setup all the drawing related elements.
	RETURN_CODE = DRAW_SETUP();

	//Wait for a key command to come in.
	char KEY_INPUT;
	while (RETURN_CODE == 0) {
		//Go to the main loop of the program.
		RETURN_CODE = MAIN_LOOP();
	}
	DRAW_SHUTDOWN();
	return RETURN_CODE;
}

int MAIN_LOOP(void) {

	DRAW_CYCLE_TIME_WINDOW();
	WIRE_POLL();
	DRAW_INPUT_WINDOW(SENSORS, LAST_CHANGE);
	DRAW_TIME();
	DRAW_SYS_DATA();
	SLEEP_MS(SLEEP_TIME);

	//Write all the changes to the screen.
	refresh();
	return 0;
}

static void SLEEP_MS(long MILLISECONDS){
       struct timespec TS_SLEEP,TS_REMAINING;
	   //Calculate the nanosecond
       long REMAINING_MILLISECONDS = (MILLISECONDS) % 1000;
       long REMAINING_NANOSECONDS = REMAINING_MILLISECONDS * 1000000;
       TS_SLEEP.tv_sec = (MILLISECONDS) / 1000;
       TS_SLEEP.tv_nsec = REMAINING_NANOSECONDS;
       nanosleep(&TS_SLEEP, &TS_REMAINING);
}

