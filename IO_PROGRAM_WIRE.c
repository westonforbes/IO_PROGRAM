//DETAILS:
//Written by Weston Forbes.
//westonforbes@automaticspring.com, westonforbes@gmail.com
//Project begin date 2021_04_13. Version control is part of file name. (FILENAME_YYYY_MM_DD{DAILY REV LETTER}.c).

//NOTE ON NAMING CONVENTION:
//Each method should start with the name of the file its contained in. For example, every method in IO_DEV_DRAW should begin with DRAW.
//This makes it easier to manually locate where methods are in the project structure.

//BASIC DESCRIPTION:
//This file handles I/O.

//Includes.
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <wiringPi.h>

//Debounce time in microseconds (1/1,000,000 seconds).
#define IGNORE_CHANGE_BELOW_USEC 100

//Define GPIO Pins.
//Note that pin mapping is confusing. There are three different ways to reference a pin.
//Its physical pin, its GPIO assignment and its WiringPi assignment.
//For defining a pin in software, use the WiringPi number. However terminal block breakout boards
//will have this labelled by its GPIO number. There is a map saved in the source code directory
//and the same map may be found at https://hackage.haskell.org/package/wiringPi
#define PIN_01 16 //GPIO 15
#define PIN_02 1  //GPIO 18
#define PIN_03 4  //GPIO 23
#define PIN_04 5  //GPIO 24
#define PIN_05 26 //GPIO 12
#define PIN_06 27 //GPIO 16
#define PIN_07 28 //GPIO 20
#define PIN_08 29 //GPIO 21

//Global variables.
//Position 0 is not used. The physical pins are 1 indexed, so this just keeps things more organized.
bool SENSORS[9] = { 0,0,0,0,0,0,0,0,0 };

//Stucture for storing the last interrupt time.
//Position 0 is a swap space and the next slots are designated to the cooresponding pins.
struct timeval LAST_CHANGE[9];


//Method prototypes.
int WIRE_SETUP(void);
void WIRE_HANDLE(int PIN);
void WIRE_POLL(void);
struct timeval WIRE_PACKAGE_TIMESTAMPS(void);

//Handles. We have to get a little tricky because the handle setup does not take any arguments.
void WIRE_HANDLE_01(void) { WIRE_HANDLE(1); }
void WIRE_HANDLE_02(void) { WIRE_HANDLE(2); }
void WIRE_HANDLE_03(void) { WIRE_HANDLE(3); }
void WIRE_HANDLE_04(void) { WIRE_HANDLE(4); }
void WIRE_HANDLE_05(void) { WIRE_HANDLE(5); }
void WIRE_HANDLE_06(void) { WIRE_HANDLE(6); }
void WIRE_HANDLE_07(void) { WIRE_HANDLE(7); }
void WIRE_HANDLE_08(void) { WIRE_HANDLE(8); }

//Method implementations.
void WIRE_HANDLE(int PIN) {
	struct timeval TIME_NOW;
	unsigned long DIFFERENCE;
	int DIR;

	//Get the time of day.
	gettimeofday(&TIME_NOW, NULL);

	//Configure some stuff depending on the calling pin.
	LAST_CHANGE[0] = LAST_CHANGE[PIN];
	LAST_CHANGE[PIN] = TIME_NOW;
	switch (PIN) {
		case 1:
			DIR = digitalRead(PIN_01);
			break;
		case 2:
			DIR = digitalRead(PIN_02);
			break;
		case 3:
			DIR = digitalRead(PIN_03);
			break;
		case 4:
			DIR = digitalRead(PIN_04);
			break;
		case 5:
			DIR = digitalRead(PIN_05);
			break;
		case 6:
			DIR = digitalRead(PIN_06);
			break;
		case 7:
			DIR = digitalRead(PIN_07);
			break;
		case 8:
			DIR = digitalRead(PIN_08);
			break;
	}

	//Time difference in microseconds.
	DIFFERENCE = (TIME_NOW.tv_sec * 1000000 + TIME_NOW.tv_usec) - (LAST_CHANGE[0].tv_sec * 1000000 + LAST_CHANGE[0].tv_usec);
	//Use the time difference as a debounce filter.
	if (DIFFERENCE > IGNORE_CHANGE_BELOW_USEC) {
		SENSORS[PIN] = DIR;
	}
}

void WIRE_POLL(void){
	SENSORS[1] = digitalRead(PIN_01);
	SENSORS[2] = digitalRead(PIN_02);
	SENSORS[3] = digitalRead(PIN_03);
	SENSORS[4] = digitalRead(PIN_04);
	SENSORS[5] = digitalRead(PIN_05);
	SENSORS[6] = digitalRead(PIN_06);
	SENSORS[7] = digitalRead(PIN_07);
	SENSORS[8] = digitalRead(PIN_08);
}

int WIRE_SETUP(void) {
	//Initialize the wiringPi library.
	wiringPiSetup();

	//Set the pins to inputs.
	pinMode(PIN_01, INPUT);
	pinMode(PIN_02, INPUT);
	pinMode(PIN_03, INPUT);
	pinMode(PIN_04, INPUT);
	pinMode(PIN_05, INPUT);
	pinMode(PIN_06, INPUT);
	pinMode(PIN_07, INPUT);
	pinMode(PIN_08, INPUT);

	//Set the internal resistors.
	pullUpDnControl(PIN_01, PUD_DOWN);
	pullUpDnControl(PIN_02, PUD_DOWN);
	pullUpDnControl(PIN_03, PUD_DOWN);
	pullUpDnControl(PIN_04, PUD_DOWN);
	pullUpDnControl(PIN_05, PUD_DOWN);
	pullUpDnControl(PIN_06, PUD_DOWN);
	pullUpDnControl(PIN_07, PUD_DOWN);
	pullUpDnControl(PIN_08, PUD_DOWN);


	//Set the intial LAST_CHANGE values.
	gettimeofday(&LAST_CHANGE[0], NULL);
	for(int i = 1; i < 9; i++){
		LAST_CHANGE[i]= LAST_CHANGE[0];
	}

	//Bind the pins to interrupts.
	wiringPiISR(PIN_01, INT_EDGE_BOTH, &WIRE_HANDLE_01);
	wiringPiISR(PIN_02, INT_EDGE_BOTH, &WIRE_HANDLE_02);
	wiringPiISR(PIN_03, INT_EDGE_BOTH, &WIRE_HANDLE_03);
	wiringPiISR(PIN_04, INT_EDGE_BOTH, &WIRE_HANDLE_04);
	wiringPiISR(PIN_05, INT_EDGE_BOTH, &WIRE_HANDLE_05);
	wiringPiISR(PIN_06, INT_EDGE_BOTH, &WIRE_HANDLE_06);
	wiringPiISR(PIN_07, INT_EDGE_BOTH, &WIRE_HANDLE_07);
	wiringPiISR(PIN_08, INT_EDGE_BOTH, &WIRE_HANDLE_08);
}
