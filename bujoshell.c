#include "bujoshell.h"

#include <locale.h>
#include <ncurses.h>
#include <stdio.h>

#include "draw.h"
#include "init.h"
#include "input.h"
#include "update.h"

int main(void) {
  /* Enable Emojis */
  setlocale(LC_CTYPE, "");

  AppData app;

  InitScreen();

  ErrorCode init_app = InitApp(&app);
  if (init_app != NO_ERROR) {
    endwin();
    fprintf(stderr, "Error initializing windows: %d\n", init_app);
    return init_app;
  }
  ErrorCode init_windows_result = InitWindows(&app);
  if (init_windows_result != NO_ERROR) {
    endwin();
    fprintf(stderr, "Error initializing windows: %d\n", init_windows_result);
    return init_windows_result;
  }

  while (app.running) {
    ErrorCode update_app = UpdateApp(&app);
    if (update_app != NO_ERROR) {
      endwin();
      fprintf(stderr, "Error updating app: %d\n", update_app);
      return update_app;
    }

    ErrorCode draw_screen = DrawScreen(&app);

    if (draw_screen != NO_ERROR) {
      endwin();
      fprintf(stderr, "Error drawing screen: %d\n", draw_screen);
      return draw_screen;
    }

    ErrorCode handling_input = HandleInputs(&app);
    if (handling_input != NO_ERROR) {
      endwin();
      fprintf(stderr, "Error handling input: %d\n", handling_input);
      return handling_input;
    }

    napms(1000 / FPS);
  }

  endwin();
  printf("Goodbye!\n");

  return 0;
}
