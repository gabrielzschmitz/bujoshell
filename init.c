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
  InitFutureLog(&app->future_log);

  app->current_hour = 0;
  app->current_minute = 0;
  app->current_day = 0;
  app->current_month = 0;
  app->current_year = 0;
  app->days_in_month = 0;

  app->input_mode = NORMAL;
  app->insert_buffer = (char *)calloc(1, sizeof(char) + 1);
  if (app->insert_buffer == NULL) return MALLOC_ERROR;
  app->user_input = 0;
  app->cursor_x = 0;
  app->cursor_y = 0;

  for (int i = 0; i < PAGE_HISTORY_SIZE; i++) app->page_history[i] = FUTURE_LOG;
  app->current_future_log = -1;
  app->show_status_bar = 1;
  app->show_key_page = 0;
  app->show_index_page = 0;

  app->width = 0;
  app->height = 0;
  GetScreenSize(app);

  return NO_ERROR;
}

/* Initialize a future log */
void InitFutureLog(FutureLogData *future_log) {
  for (int i = 0; i <= MAX_MONTHS; i++) future_log->months[i].head = NULL;
  future_log->last_id = 0;
  future_log->current_id = 0;
  DeserializeFromDB(future_log);
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

  // Insert data into the table
  sqlite3 *db = InitializeDatabase();
  if (!db) return;
  int rc = CreateTable(db);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return;
  }
  rc = InsertData(db, &app->future_log);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return;
  }
  sqlite3_close(db);
  FreeFutureLog(&app->future_log);
}
