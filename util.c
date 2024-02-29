#include "util.h"

#include <ncurses.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#include "bujoshell.h"

/* Set text foreground and background colors */
void SetColor(short int fg, short int bg, chtype attr) {
  chtype color = COLOR_PAIR(bg * PALLETE_SIZE + fg + 1);
  color |= attr;
  attrset(color);
}

/* Get the screen size */
void GetScreenSize(AppData *app) {
  getmaxyx(stdscr, app->height, app->width);
  app->middle_x = app->width / 2;
  app->middle_y = app->height / 2;
}

/* Create a border into a given window */
void CreateBorder(Window *win, short int fg, short int bg, chtype attr) {
  SetColor(fg, bg, attr);

  int start_x = win->start_x;
  int start_y = win->start_y;
  int width = win->width - 1;
  int height = win->height - 1;

  /* Corners */
  mvaddch(start_y, start_x, ACS_ULCORNER);
  mvaddch(start_y, start_x + width, ACS_URCORNER);
  mvaddch(start_y + height, start_x, ACS_LLCORNER);
  mvaddch(start_y + height, start_x + width, ACS_LRCORNER);

  /* Horizontal lines */
  int topRow = start_y;
  int bottomRow = start_y + height;
  for (int col = start_x + 1; col < start_x + width; col++) {
    mvaddch(topRow, col, ACS_HLINE);
    mvaddch(bottomRow, col, ACS_HLINE);
  }

  /* Vertical lines */
  int leftCol = start_x;
  int rightCol = start_x + width;
  for (int row = start_y + 1; row < start_y + height; row++) {
    mvaddch(row, leftCol, ACS_VLINE);
    mvaddch(row, rightCol, ACS_VLINE);
  }
}

/* Adds the current_page to the page history stack */
void AddToPageHistory(CurrentPage *page_history, CurrentPage current_page) {
  for (int i = PAGE_HISTORY_SIZE; i > 0; i--)
    page_history[i] = page_history[i - 1];

  page_history[0] = current_page;
}

/* Return 1 if the given year is a leap year, 0 if not */
int IsLeapYear(int year) {
  return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

/* Return the number of days in a month of given year */
int DaysInMonth(int year, int month) {
  if (month < 1 || month > 12) return -1;

  int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if (month == 2 && IsLeapYear(year))
    return 29;
  else
    return days[month];
}

/* Add an entry to a specific month */
void AddEntry(FutureLogData *future_log, int month_index, EntryType type,
              const char *text, const char *date) {
  future_log->current_id += 1;
  int id = future_log->current_id;

  LogEntry *new_entry = (LogEntry *)malloc(sizeof(LogEntry));
  if (new_entry == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    return;
  }

  new_entry->id = id;
  new_entry->type = type;
  new_entry->text = strdup(text);
  new_entry->date = strdup(date);
  new_entry->next = NULL;

  if (future_log->months[month_index].head == NULL)
    future_log->months[month_index].head = new_entry;
  else {
    LogEntry *current = future_log->months[month_index].head;
    while (current->next != NULL) current = current->next;
    current->next = new_entry;
  }
}

/* Function to convert EntryType enum to string */
const char *EntryTypeToString(EntryType type) {
  switch (type) {
    case TASK:
      return "TASK";
    case NOTE:
      return "NOTE";
    case EVENT:
      return "EVENT";
    case APPOINTMENT:
      return "APPOINTMENT";
    default:
      return "UNKNOWN";
  }
  return "UNKNOWN";
}

/* Return the number of entrys in given month */
int CountMonthEntrys(MonthEntry *month) {
  int entrys_count = 0;
  LogEntry *current = month->head;
  while (current != NULL) {
    entrys_count += 1;
    LogEntry *temp = current;
    current = current->next;
  }
  return entrys_count;
}

/* Free memory allocated for notes in a specific month */
void FreeMonthNotes(MonthEntry *month) {
  LogEntry *current = month->head;
  while (current != NULL) {
    LogEntry *temp = current;
    current = current->next;
    free(temp->text);
    free(temp->date);
    free(temp);
  }
}

/* Free memory allocated for the future log */
void FreeFutureLog(FutureLogData *future_log) {
  for (int i = 0; i <= MAX_MONTHS; i++) {
    FreeMonthNotes(&(future_log->months[i]));
  }
}

/* Callback function for executing SQL queries */
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  int i;
  for (i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

/* Initialize SQLite database */
sqlite3 *InitializeDatabase() {
  sqlite3 *db;
  int rc;

  rc = sqlite3_open("bujoshell.db", &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return NULL;
  }

  return db;
}

/* Create FutureLog table */
int CreateTable(sqlite3 *db) {
  char *zErrMsg = 0;
  const char *sql =
    "CREATE TABLE IF NOT EXISTS FutureLog (ID INTEGER PRIMARY KEY "
    "AUTOINCREMENT, Month INTEGER, Type TEXT, Text TEXT, Date TEXT);";
  int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return rc;
  }
  return SQLITE_OK;
}

/* Insert data into FutureLog table */
int InsertData(sqlite3 *db, const FutureLogData *future_log) {
  char *zErrMsg = 0;
  int rc;

  for (int i = 0; i <= MAX_MONTHS; i++) {
    LogEntry *current = future_log->months[i].head;
    while (current != NULL) {
      if (current->id > future_log->last_id) {
        char insert_sql[512];
        sprintf(insert_sql,
                "INSERT INTO FutureLog (ID, Month, Type, Text, Date) "
                "VALUES "
                "(%d, %d, '%s', '%s', '%s');",
                current->id, i, EntryTypeToString(current->type), current->text,
                current->date);
        rc = sqlite3_exec(db, insert_sql, callback, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
          fprintf(stderr, "SQL error: %s\n", zErrMsg);
          sqlite3_free(zErrMsg);
          return rc;
        }
      }
      current = current->next;
    }
  }
  return SQLITE_OK;
}

/* Deserialize data from SQLite database to FutureLogData struct */
void DeserializeFromDB(FutureLogData *future_log) {
  sqlite3 *db;
  sqlite3_stmt *res;
  int rc;

  db = InitializeDatabase();
  if (!db) return;

  const char *sql = "SELECT * FROM FutureLog";
  rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return;
  }

  while (sqlite3_step(res) == SQLITE_ROW) {
    int id = sqlite3_column_int(res, 0);
    if (id > future_log->last_id) {
      future_log->last_id = id;
    }

    int month = sqlite3_column_int(res, 1);
    const char *type_str = (const char *)sqlite3_column_text(res, 2);
    const char *text = (const char *)sqlite3_column_text(res, 3);
    const char *date = (const char *)sqlite3_column_text(res, 4);

    EntryType type;
    if (strcmp(type_str, "TASK") == 0) {
      type = TASK;
    } else if (strcmp(type_str, "EVENT") == 0) {
      type = EVENT;
    } else if (strcmp(type_str, "APPOINTMENT") == 0) {
      type = APPOINTMENT;
    } else {
      type = NOTE;
    }
    if (strcmp(date, "(null)") == 0) {
      AddEntry(future_log, month, type, text, NULL);
    } else
      AddEntry(future_log, month, type, text, date);
  }
  future_log->current_id = future_log->last_id;

  sqlite3_finalize(res);
  sqlite3_close(db);
}

/* Delete an entry from the database by ID */
void DeleteEntryByID(int entry_id) {
  sqlite3 *db;
  int rc;

  db = InitializeDatabase();
  if (!db) return;

  // Construct the SQL query to delete the entry with the given ID
  char delete_sql[256];
  sprintf(delete_sql, "DELETE FROM FutureLog WHERE ID = %d;", entry_id);
  char *zErrMsg = 0;

  // Execute the SQL query
  rc = sqlite3_exec(db, delete_sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return;
  }
}

/* Returns a string based in the int week day */
const char *GetDayOfWeek(int day) {
  const char *week_days[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

  if (day >= 0 && day <= 6)
    return week_days[day];
  else
    return "Invalid day";
}

/* Returns the amount of strings in array of strings */
int CountStrings(const char *strings[]) {
  int count = 0;
  while (strings[count] != NULL) {
    count++;
  }
  return (count);
}

/* Returns the text from given month and entry_index or NULL if past size */
char *GetEntryText(MonthEntry *month, int entry_index) {
  char *selected_entry = NULL;
  LogEntry *current = month->head;

  int entrys_count = 0;
  while (current != NULL) {
    if (entrys_count == entry_index) {
      selected_entry = strdup(current->text);
      break;
    }
    entrys_count += 1;
    current = current->next;
  }

  return selected_entry;
}

/* Returns the id from a given month and entry_index or -1 if past size */
int GetEntryId(MonthEntry *month, int entry_index) {
  int selected_entry = -1;
  LogEntry *current = month->head;

  int entrys_count = 0;
  while (current != NULL) {
    if (entrys_count == entry_index) {
      selected_entry = current->id;
      break;
    }
    entrys_count += 1;
    current = current->next;
  }

  return selected_entry;
}

/* Remove an entry from the FutureLogData struct by ID */
void RemoveEntryByID(FutureLogData *future_log, int entry_id) {
  for (int i = 0; i <= MAX_MONTHS; i++) {
    LogEntry *current = future_log->months[i].head;
    LogEntry *prev = NULL;

    while (current != NULL) {
      if (current->id == entry_id) {
        if (prev != NULL)
          prev->next = current->next;
        else
          future_log->months[i].head = current->next;
        free(current->date);
        free(current->text);
        free(current);
        return;
      }
      prev = current;
      current = current->next;
    }
  }
}
