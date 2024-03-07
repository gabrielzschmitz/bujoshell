#ifndef BUJOTUI_H_
#define BUJOTUI_H_

#include <ncurses.h>

#define PAGE_HISTORY_SIZE 3
#define TAB_SIZE          4
#define MAX_MONTHS        12
#define MAX_DAYS          31
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
  INVALID_INPUT,
  TOO_SMALL_SCREEN,
  INVALID_MONTH,
  INVALID_DAY,
} ErrorCode;

/* Defining entry type enum */
typedef enum { TASK, NOTE, EVENT, APPOINTMENT } EntryType;

/* Defining a structure for a note */
typedef struct LogEntry LogEntry;
struct LogEntry {
  int id;
  EntryType type;
  char *text;
  int year;
  int month;
  int day;
  struct LogEntry *next;
};

/* Defining a structure for a month */
typedef struct MonthEntry MonthEntry;
struct MonthEntry {
  LogEntry *head;
};

/* Defining a structure for the future log */
typedef struct LogData LogData;
struct LogData {
  MonthEntry months[MAX_MONTHS + 1];
  int last_id;
  int current_id;
};

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
  LogData future_log;
  LogData monthly_log;
  LogData daily_log;

  int current_hour;
  int current_minute;
  int current_week_day;
  int current_day;
  int current_month;
  int current_year;
  int days_in_month;

  int user_input;
  int cursor_x;
  int cursor_y;
  int selected_month;
  int selected_entry;
  int selected_day;
  int insert_cursor_x;
  int insert_cursor_y;
  int entry_has_date;
  int entry_type;
  int entry_day;
  int entry_month;
  char *insert_buffer;
  int entry_input;
  int deletion_popup;
  int selected_option;
  InputMode input_mode;

  CurrentPage page_history[PAGE_HISTORY_SIZE];
  int current_future_log;
  int current_monthly_log;
  Window *main_window;
  Window *floating_window;
  int width, height;
  int middle_x, middle_y;
  int show_status_bar;
  int show_key_page;
  int show_index_page;
};

#endif /* BUJOTUI_H_ */
