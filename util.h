#ifndef UTIL_H_
#define UTIL_H_

#include <ncurses.h>

#include "bujoshell.h"

/* Defining the colors pallete size */
#define PALLETE_SIZE (COLOR_WHITE - COLOR_BLACK + 1)
#define NO_COLOR     -1

/* Set text foreground and background colors */
void SetColor(short int fg, short int bg, chtype attr);

/* Get the screen size */
void GetScreenSize(AppData *app);

/* Create a border into a given window */
void CreateBorder(Window *win, short int fg, short int bg, chtype attr);

/* Adds the current_page to the page history stack */
void AddToPageHistory(CurrentPage *history, CurrentPage current_page);

/* Return 1 if the given year is a leap year, 0 if not */
int IsLeapYear(int year);

/* Return the number of days in a month of given year */
int DaysInMonth(int year, int month);

#endif /* UTIL_H_ */
