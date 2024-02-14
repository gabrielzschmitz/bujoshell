#ifndef UTIL_H_
#define UTIL_H_

#include <ncurses.h>
#include <sqlite3.h>

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

/* Deserialize data from SQLite database to FutureLogData struct */
void DeserializeFromDB(FutureLogData *future_log);

/* Insert data into FutureLog table */
int InsertData(sqlite3 *db, const FutureLogData *future_log);

/* Create FutureLog table */
int CreateTable(sqlite3 *db);

/* Initialize SQLite database */
sqlite3 *InitializeDatabase();

/* Callback function for executing SQL queries */
static int callback(void *NotUsed, int argc, char **argv, char **azColName);

/* Free memory allocated for notes in a specific month */
void FreeMonthNotes(MonthEntry *month);

/* Free memory allocated for the future log */
void FreeFutureLog(FutureLogData *future_log);

/* Function to convert EntryType enum to string */
const char *EntryTypeToString(EntryType type);

/* Add an entry to a specific month */
void AddEntry(FutureLogData *future_log, int month_index, EntryType type,
              const char *text, const char *date, const char *deadline);

/* Initialize a future log */
void InitFutureLog(FutureLogData *future_log);

#endif /* UTIL_H_ */
