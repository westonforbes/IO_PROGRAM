//DETAILS:
//Written by Weston Forbes.
//westonforbes@automaticspring.com, westonforbes@gmail.com

//NOTE ON NAMING CONVENTION:
//Each method should start with the name of the file its contained in. For example, every method in IO_DEV_DRAW should begin with DRAW.
//This makes it easier to manually locate where methods are in the project structure.

//BASIC DESCRIPTION:
//This file handles visuals, basically a terminal render engine.

//Includes.
#include <ncurses.h>
#include <string.h> 
#include <wchar.h>
#include <locale.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include <fcntl.h>

//Color definitions.
#define WHITE_BLUE 1
#define WHITE_RED 2
#define GREEN_BLUE 3
#define RED_BLUE 4
#define GREEN_BLACK 5
#define RED_BLACK 6
#define BLACK_GREEN 7
#define BLACK_RED 8

//Method prototypes.
/// <summary>
/// This method sets up all the drawing related stuff.
/// </summary>
/// <param name=""></param>
/// <returns></returns>
int DRAW_SETUP(void);

/// <summary>
/// This method draws the cycle time box (and performs the calculation).
/// </summary>
/// <param name=""></param>
void DRAW_CYCLE_TIME_WINDOW(void);

/// <summary>
/// This method draws the window that shows the sensor states.
/// </summary>
/// <param name="SENSOR"></param>
void DRAW_INPUT_WINDOW(bool SENSOR[], struct timeval TIMESTAMP[] );

/// <summary>
/// Simply draws the header of the main screen.
/// </summary>
/// <param name=""></param>
/// <returns>A return code from its call to DRAW_BOX method.</returns>
int DRAW_HEADER(void);

/// <summary>
/// This function draws a box on the screen at the given coordinates.
/// X and Y are zero index values (the upper left corner is 0,0).
/// The width and height include the borders (so a 2x2 box would be all borders).
/// </summary>
/// <param name="X">The X value (zero index).</param>
/// <param name="Y">The Y value (zero index).</param>
/// <param name="WIDTH">The width of the box, border included.</param>
/// <param name="HEIGHT">The height of the box, border included.</param>
/// <returns>
/// 1 = The box would not fit on the screen.
///	2 = The box starts off screen.
/// 3 = The box is too small to render.
/// </returns>
int DRAW_BOX(int X, int Y, int WIDTH, int HEIGHT);

/// <summary>
/// This method throws up a red warning screen.
/// </summary>
/// <param name="RETURN_CODE">The return code.</param>
void DRAW_ABORT_SCREEN(int RETURN_CODE);

/// <summary>
/// Method for anything shutdown related.
/// </summary>
void DRAW_SHUTDOWN(void);

/// <summary>
/// This method draws the time box on screen.
/// </summary>
void DRAW_TIME(void);

/// <summary>
/// This method draws the memory box on screen.
/// </summary>
void DRAW_SYS_DATA(void);

/// <summary>
/// This method does the actual lifting of getting CPU load.
/// </summary>
/// <returns>
/// The CPU load as a integer percentage.
/// </returns>
int GET_CPU_LOAD(void); 

//Method implementations.
int DRAW_SETUP(void) {
	//Setup a variable to hold return codes from called functions.
	int RETURN_CODE = 0;

	//Set locale for proper unicode extended character printing.
	setlocale(LC_ALL, "");

	//Start the Ncurses mode.
	initscr();

	//Start color mode.
	start_color();

	//Define a color pair.
	//Set the defines up at the top of file.
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_WHITE, COLOR_RED);
	init_pair(3, COLOR_GREEN, COLOR_BLUE);
	init_pair(4, COLOR_RED, COLOR_BLUE);
	init_pair(5, COLOR_GREEN, COLOR_BLACK);
	init_pair(6, COLOR_RED, COLOR_BLACK);
	init_pair(7, COLOR_BLACK, COLOR_GREEN);
	init_pair(8, COLOR_BLACK, COLOR_RED);

	//Set the main screen to the predefined color pair.
	wbkgd(stdscr, COLOR_PAIR(WHITE_BLUE));

	//Hide the cursor.
	curs_set(0);

	//Don't allow for keyboard data to be written to terminal output.
	noecho();

	//Print a header box.
	RETURN_CODE = DRAW_HEADER();

	//If there was a problem printing the header, abort.
	if (RETURN_CODE != 0)
	{
		DRAW_ABORT_SCREEN(RETURN_CODE);
		return RETURN_CODE;
	}

	//Get the dimensions of the terminal.
	int ROWS, COLUMNS;
	getmaxyx(stdscr, ROWS, COLUMNS);

	//Move the print cursor two rows from the bottom.
	mvprintw(ROWS - 1, 0, "Press Ctrl+C to quit.");
	
	return RETURN_CODE;
}

void DRAW_INPUT_WINDOW(bool SENSOR[], struct timeval TIMESTAMP[] ) {
	//Define parameters.
	int ELEMENTS = 8; //The number of sensors to list.
	int X_LOCATION = 34; //Location on screen, 0 index position. Position includes border.
	int Y_LOCATION = 7; //Location on screen, 0 index position. Position includes border.
	int WIDTH = 48; //Box width.
	int HEIGHT = 14; //Box height.
	int SENSOR_0_INDIC_X = 3; //Where to position the indicator in the box. 0 index includes border, so effectively 1 index (1 puts it right next to border).
	int SENSOR_0_INDIC_Y = 4; //Where to position the indicator in the box. 0 index includes border, so effectively 1 index (1 puts it right next to border).
	int SENSOR_SPACING = 1; //The space between each sensor indicator.
	wchar_t OFF_SENSOR_SYMBOL = 0x25C6; //Unicode symbol to indicate sensor is off.
	wchar_t ON_SENSOR_SYMBOL = 0x25C7; //Unicode symbol to indicate sensor is on.

	//Define some time related variables.
	time_t TIME_RAW;
	struct tm * TIME_STRUCTURE;
	char TIME_STRING[28];


	//Create the box.
	DRAW_BOX(X_LOCATION, Y_LOCATION, WIDTH, HEIGHT);

	//Write the title.
	char MESSAGE[] = "  STATE  SENSOR       LAST EDGE TIMESTAMP";
	mvprintw(Y_LOCATION + 2, X_LOCATION + 1, MESSAGE);

	//Write the indicators.
	for (int i = 1 ; i <= ELEMENTS; i++)
	{

		//Convert the time.
		TIME_RAW = TIMESTAMP[i].tv_sec;
		TIME_STRUCTURE = localtime(&TIME_RAW);
		strftime(TIME_STRING, 21, "%Y-%m-%d %H:%M:%S", TIME_STRUCTURE);
		sprintf(TIME_STRING, "%s.%06d",TIME_STRING, TIMESTAMP[i].tv_usec);

		//If the sensor is on.
		if (SENSOR[i])
		{
			attron(COLOR_PAIR(BLACK_GREEN));
			mvprintw(Y_LOCATION + SENSOR_0_INDIC_Y + SENSOR_SPACING * (i-1), X_LOCATION + SENSOR_0_INDIC_X, "  ON ");
			attroff(COLOR_PAIR(BLACK_GREEN));
			printw(" SENSOR %d - %s", i , TIME_STRING);
			
		}
		//If the sensor is off.
		else
		{
			attron(COLOR_PAIR(BLACK_RED));
			mvprintw(Y_LOCATION + SENSOR_0_INDIC_Y + SENSOR_SPACING * (i-1), X_LOCATION + SENSOR_0_INDIC_X, " OFF ");
			attroff(COLOR_PAIR(BLACK_RED));
			printw(" SENSOR %d - %s", i , TIME_STRING);
			
		}
	}
}

int DRAW_HEADER(void) {
	//Setup a variable to hold return codes from called functions.
	int RETURN_CODE = 0;

	//Get the dimensions of the terminal.
	int ROWS, COLUMNS;
	getmaxyx(stdscr, ROWS, COLUMNS);

	//Draw a box for the header.
	RETURN_CODE = DRAW_BOX(2, 1, COLUMNS-4, 5);

	//Define the text to print.
	char HEADER_TEXT_ROW_1[] = "Westons Sensor Input Program.";
	char HEADER_TEXT_ROW_2[] = "Using NCurses Library Wide and WiringPi.";
	char HEADER_TEXT_ROW_3[] = "2021";

	//Print the text.
	mvprintw(2, (COLUMNS - strlen(HEADER_TEXT_ROW_1)) / 2, "%s", HEADER_TEXT_ROW_1);
	mvprintw(3, (COLUMNS - strlen(HEADER_TEXT_ROW_2)) / 2, "%s", HEADER_TEXT_ROW_2);
	mvprintw(4, (COLUMNS - strlen(HEADER_TEXT_ROW_3)) / 2, "%s", HEADER_TEXT_ROW_3);

	//Exit.
	return RETURN_CODE;
}

int DRAW_BOX(int X, int Y, int WIDTH, int HEIGHT){
	//Get the dimensions of the terminal.
	int ROWS, COLUMNS;
	getmaxyx(stdscr,ROWS,COLUMNS);

	//Check to make sure the arguments will produce safe results.
	if(X+WIDTH>COLUMNS || Y + HEIGHT > ROWS) return 1;
	if(X < 0 || Y < 0) return 2;
	if(WIDTH < 2 || HEIGHT < 2) return 3;

	//Define box characters.
	wchar_t BOX_UPPER_LEFT_CORNER = 0x2554;
	wchar_t BOX_UPPER_RIGHT_CORNER = 0x2557;
	wchar_t BOX_LOWER_LEFT_CORNER = 0x255A;
	wchar_t BOX_LOWER_RIGHT_CORNER = 0x255D;
	wchar_t BOX_HORIZONTAL = 0x2550;
	wchar_t BOX_VERTICAL = 0x2551;
	
	//Move to the upper left corner of the box.
	move(Y, X);

	//Print the top row.
	printw("%lc", BOX_UPPER_LEFT_CORNER);
	for(int i = 1; i<WIDTH - 1; i++)
	{
		printw("%lc", BOX_HORIZONTAL);
	}
	printw("%lc", BOX_UPPER_RIGHT_CORNER);

	//Print the vertical border.
	for(int i=1; i<=HEIGHT - 2;i++)
	{
		mvprintw(Y+i,X,"%lc",BOX_VERTICAL);
		mvprintw(Y+i,X+WIDTH-1,"%lc",BOX_VERTICAL);
	}

	//Move to the lower left corner of the box.
	move(Y+HEIGHT-1, X);

	//Print the bottom row.
	printw("%lc", BOX_LOWER_LEFT_CORNER);
	for(int i = 1; i<WIDTH - 1; i++)
	{
		printw("%lc", BOX_HORIZONTAL);
	}
	printw("%lc", BOX_LOWER_RIGHT_CORNER);

	//Exit.
	return 0;
}

void DRAW_CYCLE_TIME_WINDOW(void) {
	//Define parameters.
	int X_LOCATION = 2; //Location on screen, 0 index position. Position includes border.
	int Y_LOCATION = 7; //Location on screen, 0 index position. Position includes border.
	int WIDTH = 30; //Box width.
	int HEIGHT = 4; //Box height.

	struct timeval TIME_NOW;
	static struct timeval TIME_LAST;
	static unsigned long DIFFERENCE;
	unsigned int MILLIONS;
	unsigned int THOUSANDS;
	unsigned int HUNDREDS;

	//Create the box.
	DRAW_BOX(X_LOCATION, Y_LOCATION, WIDTH, HEIGHT);

	//Get the time of day.
	gettimeofday(&TIME_NOW, NULL);

	//Time difference in microseconds.
	DIFFERENCE = (TIME_NOW.tv_sec * 1000000 + TIME_NOW.tv_usec) - (TIME_LAST.tv_sec * 1000000 + TIME_LAST.tv_usec);
	MILLIONS = DIFFERENCE / 1000000;
	THOUSANDS = DIFFERENCE / 1000 - MILLIONS * 1000;
	HUNDREDS = DIFFERENCE - MILLIONS * 1000000 - THOUSANDS * 1000;

	//Update the last time this method was called.
	TIME_LAST = TIME_NOW;

	//Build the string
	char DIFFERENCE_STRING[32];
	sprintf(DIFFERENCE_STRING, "%03d,%03d,%03d microseconds\0", MILLIONS, THOUSANDS, HUNDREDS);

	mvprintw(Y_LOCATION + 1, X_LOCATION + 5, "TIME BETWEEN REFRESH");
	mvprintw(Y_LOCATION + 2, X_LOCATION + 1, "                            ");
	mvprintw(Y_LOCATION + 2, X_LOCATION + 3, DIFFERENCE_STRING);


}

void DRAW_ABORT_SCREEN(int RETURN_CODE) {
	//Clear the screen.
	clear();

	//Get the dimensions of the terminal.
	int ROWS, COLUMNS;
	getmaxyx(stdscr, ROWS, COLUMNS);

	//Change the screen color to red.
	wbkgd(stdscr, COLOR_PAIR(WHITE_RED)); //Color pair is defined in SETUP_NCURSES.

	//Print the message on the center of the screen.
	char MESSAGE[] = "Problem detected. Return code:";
	mvprintw(ROWS / 2, (COLUMNS - strlen(MESSAGE)) / 2, "%s", MESSAGE);
	mvprintw(ROWS / 2 + 1, COLUMNS / 2, "%d", RETURN_CODE);

	//Move the print cursor two rows from the bottom.
	mvprintw(ROWS - 1, 0, "Press any key to exit.");

	//Write all the changes to the screen.
	refresh();

	//Wait for a key command to come in.
	getch();
	endwin();
}

void DRAW_SHUTDOWN(void) {
	//Right now there is not much here but it may expand in the future,
	//So i broke it off now in the assumption more will be added.
	endwin();
}

void DRAW_TIME(void){
	//Define parameters.
	int X_LOCATION = 2; //Location on screen, 0 index position. Position includes border.
	int Y_LOCATION = 12; //Location on screen, 0 index position. Position includes border.
	int WIDTH = 30; //Box width.
	int HEIGHT = 4; //Box height.

	//Get the current time.
	time_t TIME_RAW;
  	struct tm * TIME_STRUCTURE;
	time(&TIME_RAW);
	TIME_STRUCTURE = localtime(&TIME_RAW);
	char TIME_STRING[21];
	strftime(TIME_STRING, 21, "%Y-%m-%d %H:%M:%S", TIME_STRUCTURE);
	
	//Print the current time.
	mvprintw(Y_LOCATION + 1, X_LOCATION + 4, "CURRENT LOCAL DATE TIME");
	mvprintw(Y_LOCATION + 2, X_LOCATION + 6, "%s",TIME_STRING);

	//Create the box.
	DRAW_BOX(X_LOCATION, Y_LOCATION, WIDTH, HEIGHT);
}

void DRAW_SYS_DATA(void){
	//Define parameters.
	int X_LOCATION_MEM = 2; //Location on screen, 0 index position. Position includes border.
	int Y_LOCATION_MEM = 17; //Location on screen, 0 index position. Position includes border.
	int WIDTH_MEM = 30; //Box width.
	int HEIGHT_MEM = 4; //Box height.

	int X_LOCATION_CPU_ALL = 2; //Location on screen, 0 index position. Position includes border.
	int Y_LOCATION_CPU_ALL = 22; //Location on screen, 0 index position. Position includes border.
	int WIDTH_CPU_ALL = 30; //Box width.
	int HEIGHT_CPU_ALL = 4; //Box height.

	//Get the current cpu average usage.
	int LOAD = GET_CPU_LOAD();

	//Get the current memory usage.
	struct rusage USAGE;
	getrusage(RUSAGE_SELF,&USAGE);
	
	//Print the current memory usage.
	mvprintw(Y_LOCATION_MEM + 1, X_LOCATION_MEM + 4, "CURRENT RAM UTILIZATION");
	mvprintw(Y_LOCATION_MEM + 2, X_LOCATION_MEM + 6, "%ld KILOBYTES",USAGE.ru_maxrss);

	//Print the current CPU load.
	mvprintw(Y_LOCATION_CPU_ALL + 1, X_LOCATION_CPU_ALL + 4, "CPU LOAD (ALL PROCESSES)");
	mvprintw(Y_LOCATION_CPU_ALL + 2, X_LOCATION_CPU_ALL + 6, "%d PERCENT",LOAD);

	//Create the box.
	DRAW_BOX(X_LOCATION_MEM, Y_LOCATION_MEM, WIDTH_MEM, HEIGHT_MEM);
	DRAW_BOX(X_LOCATION_CPU_ALL, Y_LOCATION_CPU_ALL, WIDTH_CPU_ALL, HEIGHT_CPU_ALL);

}

int GET_CPU_LOAD(void) {
	int FILE_HANDLER;
	char FILE_BUFFER[1024];
	float LOAD;

	FILE_HANDLER = open("/proc/loadavg", O_RDONLY);
	if(FILE_HANDLER < 0) {
		return -1; }
	read(FILE_HANDLER, FILE_BUFFER, sizeof(FILE_BUFFER) - 1);
	sscanf(FILE_BUFFER, "%f", &LOAD);
	close(FILE_HANDLER);
	return (int)(LOAD * 100);
}

