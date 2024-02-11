#include "input.h"

#include <ncurses.h>
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

/* Handle cursor movement */
int HandleCursor(AppData *app) {
  int buffer_length = strlen(app->insert_buffer);
  switch (app->user_input) {
    case KEY_LEFT:
      if (app->cursor_x > 0) app->cursor_x--;
      return 0;
      break;
    case KEY_RIGHT:
      if (app->cursor_x < buffer_length) app->cursor_x++;
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
      app->cursor_x += 1;
      break;
    case KEY_BACKSPACE:
    case BACKSPACE:
      if (app->cursor_x > 0) app->cursor_x--;
      break;
    default:
      if (app->cursor_x < buffer_length + 1) app->cursor_x++;
  }
  return 1;
}

/* Handle user input at normal mode */
ErrorCode HandleNormalMode(AppData *app) {
  CurrentPage *current_page = &app->page_history[0];
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
    case 'N':
      if (*current_page == FUTURE_LOG)
        if (app->current_future_log > 0) app->current_future_log -= 1;
      break;
    case 'n':
      if (*current_page == FUTURE_LOG)
        if (app->current_future_log < 3) app->current_future_log += 1;
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
  return NO_ERROR;
}

/* Handle user input at insert mode */
ErrorCode HandleInsertMode(AppData *app) {
  if (app->user_input == -1) return NO_ERROR;
  ErrorCode status = NO_ERROR;

  if (!HandleCursor(app)) return status;
  switch (app->user_input) {
    case KEY_ENTER:
    case ENTER:
    case ESC:
      if (getch() != -1) break;
      app->input_mode = NORMAL;
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
      for (int i = 0; i < TAB_SIZE; i++) {
        status = InsertCharacter(app, ' ');
        if (status != NO_ERROR) return status;
        app->cursor_x += 1;
      }
      app->cursor_x -= 1;
      break;
    default:
      status = InsertCharacter(app, app->user_input);
      break;
  }

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

  if (app->cursor_x < current_length)
    for (int i = current_length; i > app->cursor_x - 1; i--)
      app->insert_buffer[i] = app->insert_buffer[i - 1];
  app->insert_buffer[app->cursor_x - 1] = input;
  app->insert_buffer[current_length + 1] = '\0';

  return NO_ERROR;
}

/* Function to remove character at cursor and shrink the buffer */
ErrorCode RemoveCharacter(AppData *app) {
  int current_length = strlen(app->insert_buffer);
  if (current_length == 0 || app->cursor_x > current_length) return NO_ERROR;

  for (int i = app->cursor_x; i < current_length; i++)
    app->insert_buffer[i] = app->insert_buffer[i + 1];
  app->insert_buffer[current_length] = '\0';
  current_length = strlen(app->insert_buffer);

  char *new_buffer = (char *)realloc(app->insert_buffer, current_length + 1);
  if (new_buffer == NULL) return MALLOC_ERROR;
  app->insert_buffer = new_buffer;

  return NO_ERROR;
}
