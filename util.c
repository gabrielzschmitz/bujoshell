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
