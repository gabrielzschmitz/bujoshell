#include "init.h"

#include <ncurses.h>
#include <stdlib.h>

#include "bujoshell.h"
#include "util.h"

/* Initialize screen with colors, enabled keyboard and another little configs */
void InitScreen(void) {
  initscr();
  if (has_colors()) {
    use_default_colors();
    int bg = 0, fg = 0;
    start_color();
    for (bg = COLOR_BLACK; bg <= COLOR_WHITE; bg++)
      for (fg = COLOR_BLACK; fg <= COLOR_WHITE; fg++)
        init_pair(bg * PALLETE_SIZE + fg + 1, fg, bg);
  }
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
  printf("\033[?1003h\n");
  noecho();
  mouseinterval(0);
  raw();
  curs_set(0);
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
}

/* Initialize variables */
ErrorCode InitApp(AppData *app) {
  app->running = 1;
  InitDataLog(&app->future_log, "FutureLog");
  InitDataLog(&app->monthly_log, "MonthlyLog");
  InitDataLog(&app->daily_log, "DailyLog");

  app->current_hour = 0;
  app->current_minute = 0;
  app->current_week_day = 0;
  app->current_day = 0;
  app->current_month = 0;
  app->current_year = 0;
  app->days_in_month = 0;

  app->input_mode = NORMAL;
  app->insert_buffer = (char *)calloc(1, sizeof(char) + 1);
  if (app->insert_buffer == NULL) return MALLOC_ERROR;
  app->entry_input = 0;
  app->entry_has_date = 1;
  app->entry_type = 0;
  app->entry_day = -1;
  app->entry_month = -1;
  app->user_input = 0;
  app->insert_cursor_x = 0;
  app->insert_cursor_y = 0;
  app->cursor_x = 1;
  app->cursor_y = 1;
  app->selected_day = -1;
  app->selected_month = -1;
  app->selected_entry = -1;
  app->deletion_popup = 0;
  app->selected_option = 0;

  for (int i = 0; i < PAGE_HISTORY_SIZE; i++) app->page_history[i] = FUTURE_LOG;
  app->current_future_log = -1;
  app->current_monthly_log = 0;
  app->current_daily_log = 0;
  app->show_status_bar = 1;
  app->show_key_page = 0;
  app->show_index_page = 0;

  app->width = 0;
  app->height = 0;
  GetScreenSize(app);

  return NO_ERROR;
}

/* Initialize a future log */
void InitDataLog(LogData *data_log, const char *db_name) {
  for (int i = 0; i <= MAX_MONTHS; i++) data_log->months[i].head = NULL;
  data_log->last_id = 0;
  data_log->current_id = 0;
  DeserializeFromDB(data_log, db_name);
}

/* Initialize a window size */
Window *InitWindowSize(int start_x, int start_y, int width, int height) {
  Window *new_window = (Window *)malloc(sizeof(Window));
  if (new_window == NULL) return NULL;

  new_window->start_y = start_y;
  new_window->start_x = start_x;
  new_window->height = height;
  new_window->width = width;
  new_window->middle_y = height / 2;
  new_window->middle_x = width / 2;

  return new_window;
}

/* Init all windows */
ErrorCode InitWindows(AppData *app) {
  app->main_window = InitWindowSize(0, 0, app->width, app->height - 1);
  if (app->main_window == NULL) return WINDOW_CREATION_ERROR;

  app->floating_window = InitWindowSize(0, 0, 0, 0);
  if (app->floating_window == NULL) {
    free(app->main_window);
    return WINDOW_CREATION_ERROR;
  }

  return NO_ERROR;
}

/* End the app and screen */
void EndApp(AppData *app) {
  app->running = 0;
  printf("\033[?1003l\n");

  if (app->insert_buffer != NULL) free(app->insert_buffer);
  app->insert_buffer = NULL;

  if (app->main_window != NULL) free(app->main_window);
  app->main_window = NULL;

  if (app->floating_window != NULL) free(app->floating_window);
  app->floating_window = NULL;

  // Insert data into the tables
  const char *future_log_db_name = "FutureLog";
  LogData *future_log_db_data = &app->future_log;
  SaveDataToDatabase(future_log_db_name, future_log_db_data);
  const char *monthly_log_db_name = "MonthlyLog";
  LogData *monthly_log_db_data = &app->monthly_log;
  SaveDataToDatabase(monthly_log_db_name, monthly_log_db_data);
  const char *daily_log_db_name = "DailyLog";
  LogData *daily_log_db_data = &app->daily_log;
  SaveDataToDatabase(daily_log_db_name, daily_log_db_data);
}
