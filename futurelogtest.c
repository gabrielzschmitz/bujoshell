#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MONTHS 12

/* Defining entry type enum */
typedef enum { TASK, NOTE, EVENT, APPOINTMENT } EntryType;

/* Defining a structure for a note */
typedef struct LogEntry LogEntry;
struct LogEntry {
  int id;
  EntryType type;
  char *date;
  char *text;
  char *deadline;
  struct LogEntry *next;
};

/* Defining a structure for a month */
typedef struct MonthEntry MonthEntry;
struct MonthEntry {
  LogEntry *head;
};

/* Defining a structure for the future log */
typedef struct FutureLogData FutureLogData;
struct FutureLogData {
  MonthEntry months[MAX_MONTHS + 1];
  int last_id;
  int current_id;
};

/* Initialize a future log */
void InitFutureLog(FutureLogData *future_log) {
  for (int i = 0; i <= MAX_MONTHS; i++) future_log->months[i].head = NULL;
  future_log->last_id = 0;
  future_log->current_id = 0;
}

/* Add an entry to a specific month */
void AddEntry(FutureLogData *future_log, int month_index, EntryType type,
              const char *text, const char *date, const char *deadline) {
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
  if (deadline != NULL)
    new_entry->deadline = strdup(deadline);
  else if (strcmp(deadline, "(null)") != 0)
    new_entry->deadline = strdup(deadline);
  else
    new_entry->deadline = NULL;
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

/* Display all notes in a specific month */
void DisplayMonthNotes(const MonthEntry *month, int month_index) {
  printf("Month %d:\n", month_index);
  LogEntry *current = month->head;
  while (current != NULL) {
    printf("ID: %d\n", current->id);
    printf("Type: %s\n", EntryTypeToString(current->type));
    printf("Text: %s\n", current->text);
    if (current->deadline == NULL) {
      printf("Date: %s\n\n", current->date);
    } else if (strcmp(current->deadline, "(null)") != 0) {
      printf("Date: %s\n", current->date);
      printf("Deadline: %s\n\n", current->deadline);
    } else
      printf("Date: %s\n\n", current->date);
    current = current->next;
  }
  printf("\n");
}

/* Display all notes in the future log */
void DisplayFutureLog(const FutureLogData *future_log) {
  for (int i = 0; i <= MAX_MONTHS; i++)
    if (future_log->months[i].head != NULL)
      DisplayMonthNotes(&(future_log->months[i]), i);
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

  rc = sqlite3_open("future_log.db", &db);
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
    "AUTOINCREMENT, Month INTEGER, Type TEXT, Text TEXT, Date TEXT, Deadline "
    "TEXT);";
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
                "INSERT INTO FutureLog (ID, Month, Type, Text, Date, Deadline) "
                "VALUES "
                "(%d, %d, '%s', '%s', '%s', '%s');",
                current->id, i, EntryTypeToString(current->type), current->text,
                current->date, current->deadline);
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
    const char *deadline =
      (const char *)sqlite3_column_text(res, 5);  // Fetch deadline

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

    AddEntry(future_log, month, type, text, date, deadline);  // Pass deadline
  }

  sqlite3_finalize(res);
  sqlite3_close(db);
}

int main() {
  FutureLogData future_log;
  InitFutureLog(&future_log);

  /* Deserialize data from SQLite database */
  DeserializeFromDB(&future_log);
  future_log.current_id = future_log.last_id;

  // Adding future log entrys
  // AddEntry(&future_log, 1, TASK, "Complete project proposal",
  //         "2024-01-07,04:20", "2024-01-08");
  // AddEntry(&future_log, 1, EVENT, "Birthday party", "2024-01-15,10:20",
  // NULL); AddEntry(&future_log, 2, APPOINTMENT, "Dentist appointment",
  //         "2024-02-12,20:40", NULL);

  // Insert data into the table
  sqlite3 *db = InitializeDatabase();
  if (!db) return 1;
  int rc = CreateTable(db);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 1;
  }
  rc = InsertData(db, &future_log);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return 1;
  }

  // Displaying the contents of the database
  DisplayFutureLog(&future_log);
  sqlite3_close(db);

  FreeFutureLog(&future_log);

  return 0;
}
