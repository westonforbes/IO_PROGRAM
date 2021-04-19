//Command to copy files to target.
//scp -r C:\Users\wforbes\Documents\SOFTWARE\RASPI_IO\IO_PROGRAM_2021_04_14B.d admin@UNIX001:PROGRAMS.d/IO_PROG.d/

//Compile and run on target.
//gcc SOURCE_CODE.d/IO_DEV_MAIN_2021_04_14A.c -lncursesw -lwiringPi -o COMPILED.d/IO_DEV_2021_04_14A ; ./COMPILED.d/IO_DEV_2021_04_14A

//DETAILS:
//Written by Weston Forbes.
//westonforbes@automaticspring.com, westonforbes@gmail.com
//Project begin date 2021_04_13. Version control is part of file name. (FILENAME_YYYY_MM_DD{DAILY REV LETTER}.c).

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

//Method prototypes.
/// <summary>
/// This is the meat and potatoes function.
/// </summary>
int MAIN_LOOP(void);

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
		DRAW_MEMORY();
		//DRAW_CPU_LOAD();
	//TODO implement nanosleep routine.
	//Write all the changes to the screen.
	refresh();
	return 0;
}

