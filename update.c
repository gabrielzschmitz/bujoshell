#include "update.h"

#include <ncurses.h>
#include <time.h>

#include "bujoshell.h"
#include "util.h"

/* Update variables */
ErrorCode UpdateApp(AppData *app) {
  GetScreenSize(app);

  ErrorCode update_main_window =
    UpdateWindowSize(&app->main_window, 0, 0, app->width, app->height - 1);
  if (update_main_window != NO_ERROR) return update_main_window;

  ErrorCode update_current_time = UpdateTime(app);
  if (update_current_time != NO_ERROR) return update_current_time;

  if (app->show_status_bar == 0 && app->show_index_page == 0) {
    ErrorCode resetting_floating_window =
      UpdateWindowSize(&app->floating_window, 0, 0, 0, 0);
    if (resetting_floating_window != NO_ERROR) return resetting_floating_window;
  }

  if (app->entry_input < 1) {
    ErrorCode resetting_floating_window =
      UpdateWindowSize(&app->floating_window, 0, 0, 0, 0);
    if (resetting_floating_window != NO_ERROR) return resetting_floating_window;
  }
  return NO_ERROR;
}

/* Update a window size */
ErrorCode UpdateWindowSize(Window **win, int start_x, int start_y, int width,
                           int height) {
  if (*win == NULL) return WINDOW_CREATION_ERROR;
  (*win)->start_y = start_y;
  (*win)->start_x = start_x;
  (*win)->height = height;
  (*win)->width = width;
  (*win)->middle_y = height / 2;
  (*win)->middle_x = width / 2;

  return NO_ERROR;
}

/* Update current hour and minute */
ErrorCode UpdateTime(AppData *app) {
  time_t now;
  struct tm *now_tm;

  now = time(NULL);
  now_tm = localtime(&now);

  app->current_hour = now_tm->tm_hour;
  app->current_minute = now_tm->tm_min;
  app->current_day = now_tm->tm_mday;
  app->current_week_day = now_tm->tm_wday;
  app->current_month = now_tm->tm_mon + 1;
  app->current_year = now_tm->tm_year + 1900;
  app->days_in_month = DaysInMonth(app->current_year, app->current_month);

  if (app->selected_month == -1) app->selected_month = app->current_month;
  if (app->selected_entry == -1)
    app->selected_entry =
      CountMonthEntrys(&app->future_log.months[app->current_month]) - 1;

  if (app->entry_day == -1) app->entry_day = app->current_day;
  if (app->entry_month == -1) app->entry_month = app->current_month;
  if (app->current_future_log == -1) {
    if (app->current_month >= 1 && app->current_month <= 3)
      app->current_future_log = 0;
    else if (app->current_month >= 4 && app->current_month <= 6)
      app->current_future_log = 1;
    else if (app->current_month >= 7 && app->current_month <= 9)
      app->current_future_log = 2;
    else if (app->current_month >= 10 && app->current_month <= 12)
      app->current_future_log = 3;
    else
      return INVALID_MONTH;
  }

  return NO_ERROR;
}

/* Creates a floating window and empty it's content */
ErrorCode CreateFloatingWindow(Window **window, int x, int y, int width,
                               int height) {
  ErrorCode update_main_window = UpdateWindowSize(window, x, y, width, height);
  if (update_main_window != NO_ERROR) return WINDOW_CREATION_ERROR;

  CreateBorder(*window, COLOR_WHITE, NO_COLOR, A_BOLD);

  const int end_width = (*window)->width + (*window)->start_x - 2;
  const int end_height = (*window)->height + (*window)->start_y - 2;
  const int start_x = (*window)->start_x + 1;
  const int start_y = (*window)->start_y + 1;

  for (int i = start_y; i < end_height + 1; i++)
    for (int j = start_x; j < end_width + 1; j++) mvaddch(i, j, ' ');

  return NO_ERROR;
}
