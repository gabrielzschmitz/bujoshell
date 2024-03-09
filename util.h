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

/* Deserialize data from SQLite database to LogData struct */
void DeserializeFromDB(LogData *data_log, const char *db_name);

/* Delete an entry from the database by ID */
void DeleteEntryByID(int entry_id, const char *db_name);

/* Insert data into Log table */
int InsertData(sqlite3 *db, const LogData *data_log, const char *db_name);

/* Create database table with given name */
int CreateTable(sqlite3 *db, const char *table_name);

/* Initialize SQLite database */
sqlite3 *InitializeDatabase();

/* Callback function for executing SQL queries */
static int callback(void *NotUsed, int argc, char **argv, char **azColName);

/* Return the number of entrys in given month */
int CountMonthEntrys(MonthEntry *month);

/* Return the number of entrys in given month without day */
int CountMonthEntrysWithoutDay(MonthEntry *month);

/* Free memory allocated for notes in a specific month */
void FreeMonthNotes(MonthEntry *month);

/* Free memory allocated for a data log */
void FreeLog(LogData *data_log);

/* Function to convert EntryType enum to string */
const char *EntryTypeToString(EntryType type);

/* Function to convert EntryType enum to symbol */
const char *EntryTypeToSymbol(EntryType type);

/* Converts string entry type to enum entry type */
EntryType StringtoEntryType(const char *type_str);

/* Add an entry to a specific month */
void AddEntry(LogData *data_log, EntryType type, const char *text,
              const int year, const int month, const int day);

/* Returns a string based in the int week day */
const char *GetDayOfWeek(int day);

/* Returns the amount of strings in array of strings */
int CountStrings(char *strings[]);

/* Returns the amount of entrys in array of entrys */
int CountEntrys(LogEntry *entrys[]);

/* Returns the text from given month and entry_index or NULL if dont find */
char *GetEntryText(MonthEntry *month, int entry_index);

/* Returns the text from given month and day or NULL if dont find */
char *GetEntryTextByDay(MonthEntry *month, int n_month, int day);

/* Returns the id from a given month and entry_index or -1 if dont find */
int GetEntryId(MonthEntry *month, int entry_index);

/* Returns the id from a given month and day or -1 if dont find */
int GetEntryIdByDay(MonthEntry *month, int n_month, int day);

/* Gets the a entrys in a given month by its id */
LogEntry *GetEntryByID(MonthEntry *month, int id);

/* Gets the a list of all the entrys in a given day and month */
LogEntry **GetEntrysOfDay(MonthEntry *month, int day);

/* Adds a new entry to a given list of entrys */
void AddToEntrysList(LogEntry ***list, int *num_entries, LogEntry *new_entry);

/* Adds a new entry to a given list of strings */
void AddToStringList(char ***list, int *num_entries, const char *new_entry);

/* Remove an entry from the LogData struct by ID */
void RemoveEntryByID(LogData *data_log, int month, int entry_id);

/* Save given struct data to a given database created if non existed*/
void SaveDataToDatabase(const char *db_name, LogData *db_data);

#endif /* UTIL_H_ */
