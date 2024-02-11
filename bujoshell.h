#ifndef BUJOTUI_H_
#define BUJOTUI_H_

#include <ncurses.h>

#define PAGE_HISTORY_SIZE 3
#define TAB_SIZE          4
#define MAX_MONTHS        12
#define FPS               60

/* Defining input mode enum */
typedef enum { NORMAL, INSERT, COMMAND } InputMode;

/* Defining page type enum */
typedef enum { FUTURE_LOG, MONTHLY_LOG, DAILY_LOG } CurrentPage;

/* Defining error handling enum */
typedef enum {
  NO_ERROR,
  WINDOW_CREATION_ERROR,
  MALLOC_ERROR,
  TOO_SMALL_SCREEN,
  INVALID_MONTH,
  INVALID_DAY
} ErrorCode;

/* Defining the window struct */
typedef struct Window Window;
struct Window {
  int start_x, start_y;
  int height, width;
  int middle_x, middle_y;
};

/* Defining the app struct */
typedef struct AppData AppData;
struct AppData {
  int running;

  int current_hour;
  int current_minute;
  int current_day;
  int current_month;
  int current_year;
  int days_in_month;

  int user_input;
  int cursor_x;
  int cursor_y;
  char *insert_buffer;
  InputMode input_mode;

  CurrentPage page_history[PAGE_HISTORY_SIZE];
  int current_future_log;
  Window *main_window;
  Window *floating_window;
  int width, height;
  int middle_x, middle_y;
  int show_status_bar;
  int show_key_page;
  int show_index_page;
};

#endif /* BUJOTUI_H_ */
