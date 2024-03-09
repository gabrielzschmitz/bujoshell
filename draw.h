#ifndef DRAW_H_
#define DRAW_H_

#include <ncurses.h>

#include "bujoshell.h"

/* Print at screen */
ErrorCode DrawScreen(AppData *app);

/* Show the status bar */
ErrorCode ShowStatusBar(AppData *app);

/* Show the page of symbols */
ErrorCode ShowKeyPage(AppData *app);

/* Show index of pages */
ErrorCode ShowIndexPage(AppData *app);

/* Display the future log page */
ErrorCode DrawFutureLog(AppData *app);

/* Display the speficied month at the speficied location */
ErrorCode RenderMonth(AppData *app, int month, int start_x, int start_y,
                      int sel_day);

/* Display the monthly log page */
ErrorCode DrawMonthlyLog(AppData *app);

/* Display up to max_entries notes in a specific month within the specified
 * boundaries */
void DisplayMonthLogEntrys(AppData *app, const MonthEntry *month, int start_x,
                           int start_y, int end_x);

/* Display the days of the current month in a vertical line
 * at the speficied location */
ErrorCode RenderDays(AppData *app, int start_x, int start_y, int day_of_weak,
                     int end_x);

ErrorCode RenderTasks(AppData *app, const MonthEntry *month, int start_x,
                      int start_y, int end_x);

/* Display terminal size error */
void DisplayTerminalSizeError(Window *window);

/* Display vertical line */
void DrawVerticalLine(int start_y, int end_height, int center_x,
                      int current_index, int total_count, int show_status_bar);

/* Display types of entry */
void DisplayEntryOptions(const int start_x, const int start_y, const char type);

/* Display up to max_entries notes in a specific month within the specified
 * boundaries */
void DisplayMonthNotes(AppData *app, const MonthEntry *month, int month_index,
                       int start_x, int start_y, int end_x, int end_y,
                       int max_entries);

/* Display the daily log page */
ErrorCode DrawDailyLog(AppData *app);

/* Draws a day log entry with the day and all tasks/notes return the new y */
ErrorCode DisplayDailyLogs(AppData *app, int start_y, int start_x,
                           MonthEntry *month, int month_index, int current_day,
                           int day_of_week);

/* Draws a day log entry with the day and all tasks/notes return the new y */
int DrawDayLogEntry(int start_y, int start_x, int current_day,
                    const char *week_day, LogEntry **entrys,
                    LogEntry *selected_entry);

/* Draws insert new entry */
ErrorCode DrawInsertEntry(AppData *app);

/* Draws the deletion popup at floating window */
ErrorCode DisplayDeletionPopup(AppData *app);

#endif /* DRAW_H_ */
