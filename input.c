#include "input.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bujoshell.h"
#include "init.h"
#include "util.h"

/* Handle user input and app state */
ErrorCode HandleInputs(AppData *app) {
  ErrorCode status = NO_ERROR;
  ESCDELAY = 25;
  app->user_input = getch();

  if (app->input_mode == NORMAL)
    status = HandleNormalMode(app);
  else if (app->input_mode == INSERT)
    status = HandleInsertMode(app);
  else if (app->input_mode == COMMAND)
    status = HandleCommandMode(app);

  flushinp();
  return status;
}

/* Handle cursor movement at normal mode */
int HandleNormalCursor(AppData *app) {
  Window *window = app->main_window;
  const int start_x = window->start_x + 1;
  const int start_y = window->start_y + 1;
  const int end_width = window->width + start_x - 3;
  const int end_height = window->height + start_y - 3;

  int cursor_x = app->cursor_x;
  int cursor_y = app->cursor_y;

  switch (app->user_input) {
    case KEY_LEFT:
    case 'h':
      if (cursor_x > start_x) app->cursor_x--;
      return 0;
      break;
    case KEY_DOWN:
    case 'j':
      if (cursor_y < end_height) app->cursor_y++;
      return 0;
      break;
    case KEY_UP:
    case 'k':
      if (cursor_y > start_y) app->cursor_y--;
      return 0;
      break;
    case KEY_RIGHT:
    case 'l':
      if (cursor_x < end_width) app->cursor_x++;
      return 0;
      break;
    default:
      break;
  }
  return 1;
}

int HandleSelectedEntry(AppData *app) {
  int max_entrys =
    CountMonthEntrys(&app->future_log.months[app->selected_month]) - 1;
  int new_max = 0;
  switch (app->user_input) {
    case KEY_LEFT:
    case 'h':
      if (app->selected_month > 0) app->selected_month--;
      new_max =
        CountMonthEntrys(&app->future_log.months[app->selected_month]) - 1;
      if (app->selected_entry > new_max) app->selected_entry = new_max;
      return 0;
      break;
    case KEY_DOWN:
    case 'j':
      if (app->selected_entry < max_entrys) app->selected_entry++;
      return 0;
      break;
    case KEY_UP:
    case 'k':
      if (app->selected_entry > 0) app->selected_entry--;
      return 0;
      break;
    case KEY_RIGHT:
    case 'l':
      if (app->selected_month < 12) app->selected_month++;
      new_max =
        CountMonthEntrys(&app->future_log.months[app->selected_month]) - 1;
      if (app->selected_entry > new_max) app->selected_entry = new_max;
      return 0;
      break;
    default:
      break;
  }
  return 1;
}

/* Handle cursor movement at insert mode */
int HandleInsertCursor(AppData *app) {
  int buffer_length = strlen(app->insert_buffer);
  switch (app->user_input) {
    case KEY_LEFT:
      if (app->insert_cursor_x > 0) app->insert_cursor_x--;
      return 0;
      break;
    case KEY_RIGHT:
      if (app->insert_cursor_x < buffer_length) app->insert_cursor_x++;
      return 0;
      break;
    case KEY_ENTER:
    case ENTER:
    case ESC:
    case KEY_STAB:
    case KEY_BTAB:
    case KEY_CTAB:
    case KEY_CATAB:
      break;
    case TAB:
      if (strlen(app->insert_buffer) < 42) app->insert_cursor_x += 1;
      break;
    case KEY_BACKSPACE:
    case BACKSPACE:
      if (app->insert_cursor_x > 0) app->insert_cursor_x--;
      break;
    default:
      if (app->insert_cursor_x < buffer_length + 1) app->insert_cursor_x++;
  }
  return 1;
}

/* Handle input at popups */
int HandlePopupInput(AppData *app) {
  if (app->deletion_popup != 1) return 1;

  int max_options = -1;
  if (app->deletion_popup == 1) max_options = 1;

  switch (app->user_input) {
    case KEY_LEFT:
    case 'h':
      if (app->selected_option > 0) app->selected_option--;
      return 0;
      break;
    case KEY_RIGHT:
    case 'l':
      if (app->selected_option < max_options) app->selected_option++;
      return 0;
      break;
    case KEY_ENTER:
    case ENTER:
      app->deletion_popup = 0;
      if (app->selected_option == 1) {
        int id_to_delete = GetEntryId(
          &app->future_log.months[app->selected_month], app->selected_entry);
        DeleteEntryByID(id_to_delete);
        RemoveEntryByID(&app->future_log, id_to_delete);
        if (app->selected_entry != 0) app->selected_entry--;
      }
      return 0;
      break;
    default:
      break;
  }

  return 1;
}

/* Handle user input at normal mode */
ErrorCode HandleNormalMode(AppData *app) {
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
  CurrentPage *current_page = &app->page_history[0];

  if (app->deletion_popup) {
    if (HandlePopupInput(app)) return NO_ERROR;
  } else if (app->entry_input == 0) {
    if (!HandleSelectedEntry(app)) return NO_ERROR;
    // if (!HandleNormalCursor(app)) return NO_ERROR;
    switch (app->user_input) {
      case ESC:
        if (getch() != -1) break;
        app->show_key_page = 0;
        app->show_index_page = 0;
        if (*current_page != app->page_history[1])
          AddToPageHistory(current_page, app->page_history[1]);
        break;
      case '2':
      case 'f':
        if (*current_page != FUTURE_LOG)
          AddToPageHistory(current_page, FUTURE_LOG);
        break;
      case '3':
      case 'm':
        if (*current_page != MONTHLY_LOG)
          AddToPageHistory(current_page, MONTHLY_LOG);
        break;
      case '4':
      case 'd':
        if (*current_page != DAILY_LOG)
          AddToPageHistory(current_page, DAILY_LOG);
        break;
      case 'N':
        if (*current_page == FUTURE_LOG)
          if (app->current_future_log > 0) app->current_future_log -= 1;
        break;
      case 'n':
        if (*current_page == FUTURE_LOG)
          if (app->current_future_log < 3) app->current_future_log += 1;
        break;
      case 'a':
        app->entry_input = 1;
        app->entry_month = app->selected_month;
        break;
      case 'r':
        if (app->deletion_popup != 1 &&
            CountMonthEntrys(&app->future_log.months[app->selected_month]) >
              0) {
          app->selected_option = 0;
          app->deletion_popup = 1;
        }
        break;
      case 'i':
        app->input_mode = INSERT;
        break;
      case ':':
      case '/':
        app->input_mode = COMMAND;
        break;
      case 'q':
      case CTRLC:
        EndApp(app);
        break;
      case '0':
      case '?':
        app->show_key_page ^= 1;
        break;
      case '1':
        app->show_index_page ^= 1;
        break;
      case 'B':
        app->show_status_bar ^= 1;
        break;
    }
  } else {
    switch (app->user_input) {
      case KEY_LEFT:
      case 'h':
        if (app->entry_type > 0 && app->entry_input == 1) app->entry_type -= 1;
        if (app->entry_has_date > 0 && app->entry_input == 3)
          app->entry_has_date -= 1;
        if (app->entry_day > 1 && app->entry_input == 4) app->entry_day -= 1;
        if (app->entry_day == 1 && app->entry_input == 4 &&
            app->entry_month > 1) {
          app->entry_month -= 1;
          app->entry_day = DaysInMonth(app->current_year, app->entry_month);
        }
        break;
      case KEY_DOWN:
      case 'j':
        if (app->entry_day + 7 <
              DaysInMonth(app->current_year, app->entry_month) &&
            app->entry_input == 4)
          app->entry_day += 7;
        else if (app->entry_input == 4)
          app->entry_day = DaysInMonth(app->current_year, app->entry_month);
        break;
      case KEY_UP:
      case 'k':
        if (app->entry_day - 7 > 1 && app->entry_input == 4)
          app->entry_day -= 7;
        else if (app->entry_input == 4)
          app->entry_day = 1;
        break;
      case KEY_RIGHT:
      case 'l':
        if (app->entry_type < 3 && app->entry_input == 1) app->entry_type += 1;
        if (app->entry_has_date < 1 && app->entry_input == 3)
          app->entry_has_date += 1;
        if (app->entry_day < DaysInMonth(app->current_year, app->entry_month) &&
            app->entry_input == 4)
          app->entry_day += 1;
        if (app->entry_day ==
              DaysInMonth(app->current_year, app->entry_month) &&
            app->entry_input == 4 && app->entry_month < 12) {
          app->entry_month += 1;
          app->entry_day = 1;
        }
        break;
      case ENTER:
      case KEY_ENTER:
        if (app->entry_input == 1) {
          app->entry_input += 1;
          app->input_mode = INSERT;
        } else if (app->entry_input == 3) {
          app->entry_input += 1;
          app->input_mode = NORMAL;
        } else if (app->entry_input == 4) {
          if (app->entry_has_date == 1) {
            char date[6];
            snprintf(date, 6, "%02d/%02d", app->entry_day, app->entry_month);
            AddEntry(&app->future_log, app->entry_month, app->entry_type,
                     app->insert_buffer, date);
          } else {
            char date[3];
            snprintf(date, 3, "%02d", app->entry_month);
            AddEntry(&app->future_log, app->entry_month, app->entry_type,
                     app->insert_buffer, date);
          }
          app->entry_input = 0;
          app->entry_day = -1;
          app->entry_month = -1;
          free(app->insert_buffer);
          app->insert_buffer = (char *)calloc(1, sizeof(char) + 1);
          app->insert_cursor_x = 0;
          app->selected_entry =
            CountMonthEntrys(&app->future_log.months[app->selected_month]) - 1;
        }
        break;
      case 'N':
        if (app->entry_month > 1) app->entry_month -= 1;
        break;
      case 'n':
        if (app->entry_month < 12) app->entry_month += 1;
        break;
      case '0':
      case '?':
        app->show_key_page ^= 1;
        break;
      case '1':
        app->show_index_page ^= 1;
        break;
      case TAB:
        app->entry_input = (app->entry_input % 4) + 1;
        break;
      case ESC:
        app->entry_day = -1;
        app->entry_month = -1;
        free(app->insert_buffer);
        app->insert_buffer = (char *)calloc(1, sizeof(char) + 1);
        app->input_mode = NORMAL;
        app->entry_input = 0;
        break;
      case 'q':
      case CTRLC:
        EndApp(app);
        break;
      default:
        break;
    }
  }
  return NO_ERROR;
}

/* Return INVALID_INPUT if a function key is detected */
ErrorCode IgnoreFunctionKeys(AppData *app) {
  ErrorCode status = NO_ERROR;
  switch (app->user_input) {
    case KEY_MOUSE:
      status = INVALID_INPUT;
      break;
    case KEY_RESIZE:
      status = INVALID_INPUT;
      break;
    case KEY_F(1):
      status = INVALID_INPUT;
      break;
    case KEY_F(2):
      status = INVALID_INPUT;
      break;
    case KEY_F(3):
      status = INVALID_INPUT;
      break;
    case KEY_F(4):
      status = INVALID_INPUT;
      break;
    case KEY_F(5):
      status = INVALID_INPUT;
      break;
    case KEY_F(6):
      status = INVALID_INPUT;
      break;
    case KEY_F(7):
      status = INVALID_INPUT;
      break;
    case KEY_F(8):
      status = INVALID_INPUT;
      break;
    case KEY_F(9):
      status = INVALID_INPUT;
      break;
    case KEY_F(10):
      status = INVALID_INPUT;
      break;
    case KEY_F(11):
      status = INVALID_INPUT;
      break;
    case KEY_F(12):
      status = INVALID_INPUT;
      break;
  }
  return status;
}

/* Handle user input at insert mode */
ErrorCode HandleInsertMode(AppData *app) {
  if (app->user_input == -1) return NO_ERROR;
  if (IgnoreFunctionKeys(app) != NO_ERROR) return NO_ERROR;
  ErrorCode status = NO_ERROR;

  if (!HandleInsertCursor(app)) return status;
  switch (app->user_input) {
    case ESC:
      if (getch() != -1) break;
      app->input_mode = NORMAL;
      break;
    case KEY_ENTER:
    case ENTER:
      if (app->entry_input == 2) {
        app->input_mode = NORMAL;
        app->entry_input++;
      }
      break;
    case KEY_BACKSPACE:
    case BACKSPACE:
      status = RemoveCharacter(app);
      break;
    case KEY_STAB:
    case KEY_BTAB:
    case KEY_CTAB:
    case KEY_CATAB:
      break;
    case TAB:
      if (strlen(app->insert_buffer) < 42) {
        for (int i = 0; i < TAB_SIZE; i++) {
          status = InsertCharacter(app, ' ');
          if (status != NO_ERROR) return status;
          app->insert_cursor_x += 1;
        }
        app->insert_cursor_x -= 1;
      }
      break;
    default:
      if (strlen(app->insert_buffer) < 42)
        status = InsertCharacter(app, app->user_input);
      break;
  }
  if (status == INVALID_INPUT) return status;

  return status;
}

/* Handle user input at command mode */
ErrorCode HandleCommandMode(AppData *app) {
  switch (app->user_input) {
    case ESC:
      if (getch() != -1) break;
      app->input_mode = NORMAL;
      break;
  }
  return NO_ERROR;
}

/* Function to get the app->user_input and insert into a growing buffer */
ErrorCode InsertCharacter(AppData *app, char input) {
  int current_length = strlen(app->insert_buffer);

  char *new_buffer = (char *)realloc(app->insert_buffer, current_length + 2);
  if (new_buffer == NULL) return MALLOC_ERROR;
  app->insert_buffer = new_buffer;

  if (app->insert_cursor_x < current_length)
    for (int i = current_length; i > app->insert_cursor_x - 1; i--)
      app->insert_buffer[i] = app->insert_buffer[i - 1];
  app->insert_buffer[app->insert_cursor_x - 1] = input;
  app->insert_buffer[current_length + 1] = '\0';

  return NO_ERROR;
}

/* Function to remove character at cursor and shrink the buffer */
ErrorCode RemoveCharacter(AppData *app) {
  int current_length = strlen(app->insert_buffer);
  if (current_length == 0 || app->insert_cursor_x > current_length)
    return NO_ERROR;

  for (int i = app->insert_cursor_x; i < current_length; i++)
    app->insert_buffer[i] = app->insert_buffer[i + 1];
  app->insert_buffer[current_length] = '\0';
  current_length = strlen(app->insert_buffer);

  char *new_buffer = (char *)realloc(app->insert_buffer, current_length + 1);
  if (new_buffer == NULL) return MALLOC_ERROR;
  app->insert_buffer = new_buffer;

  return NO_ERROR;
}

/* Separate all the input entry into the correct variables */
ErrorCode SeparateEntryVariables(const char *input, char *type, char **text,
                                 int *dd, int *mm) {
  char *token;
  char *lastSpace;

  *type = *input;
  lastSpace = strrchr(input, ' ');
  if (lastSpace == NULL) {
    fprintf(stderr, "Invalid input format.\n");
    return INVALID_INPUT;
  }
  size_t textLength = lastSpace - input - 2;

  *text = (char *)malloc(textLength + 1);
  if (*text == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    return MALLOC_ERROR;
  }
  strncpy(*text, input + 2, textLength);
  (*text)[textLength] = '\0';

  if (sscanf(lastSpace + 1, "%d/%d", dd, mm) != 2) {
    *dd = -1;
    *mm = -1;
    lastSpace = strrchr(input, '\0');
    if (lastSpace == NULL) {
      fprintf(stderr, "Invalid input format.\n");
      return INVALID_INPUT;
    }
    *lastSpace = '\0';
  }
  return NO_ERROR;
}

/* Handle all entry inputting */
ErrorCode InputEntry(AppData *app) {
  ErrorCode status = NO_ERROR;
  if (app->input_mode != INSERT) return status;
  if (app->user_input == -1) return status;

  EntryType type;
  if (app->page_history[0] == FUTURE_LOG) {
    char entry_type = app->insert_buffer[0];

    switch (entry_type) {
      case '\0':
      case 't':
      case 'T':
        type = TASK;
        break;
      case 'n':
      case 'N':
        type = NOTE;
        break;
      case 'a':
      case 'A':
        type = APPOINTMENT;
        break;
      case 'e':
      case 'E':
        type = EVENT;
        break;
      default:
        return NO_ERROR;
    }
  }

  if (app->user_input == KEY_ENTER || app->user_input == ENTER) {
    char type;
    char *text;
    int dd, mm;
    SeparateEntryVariables(app->insert_buffer, &type, &text, &dd, &mm);

    char *date = (char *)malloc(sizeof(char) * 6);
    if (dd != -1 && mm != -1)
      snprintf(date, 6, "%02d/%02d", dd, mm);
    else {
      free(date);
      date = NULL;
    }
    AddEntry(&app->future_log, app->current_month, type, text, date);

    free(text);
    free(date);
    free(app->insert_buffer);
    app->insert_buffer = (char *)calloc(1, sizeof(char) + 1);
    app->input_mode = NORMAL;
    app->insert_cursor_x = 0;
    app->selected_entry =
      CountMonthEntrys(&app->future_log.months[app->selected_month]) - 1;
  }

  return status;
}
