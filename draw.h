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
ErrorCode RenderMonth(AppData *app, int month, int start_x, int start_y);

/* Display the future log page */
ErrorCode DrawMonthlyLog(AppData *app);

/* Display the days of the current month in a vertical line
 * at the speficied location */
ErrorCode RenderDays(AppData *app, int start_x, int start_y, int day_of_weak);

/* Display terminal size error */
void DisplayTerminalSizeError(Window *window);

/* Display vertical line */
void DrawVerticalLine(int start_y, int end_height, int center_x,
                      int current_index, int total_count);

/* Display up to max_entries notes in a specific month within the specified
 * boundaries */
void DisplayMonthNotes(const MonthEntry *month, int start_x, int start_y,
                       int end_x, int end_y, int max_entries);

/* Display all notes in the future log */
void DisplayFutureLog(const FutureLogData *future_log);

#endif /* DRAW_H_ */
