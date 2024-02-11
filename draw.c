#include "draw.h"

#include <ncurses.h>
#include <string.h>
#include <time.h>

#include "bujoshell.h"
#include "config.h"
#include "update.h"
#include "util.h"

/* Print at screen */
ErrorCode DrawScreen(AppData *app) {
  erase();

  ErrorCode draw_screen = NO_ERROR;
  draw_screen = DrawFutureLog(app);
  if (draw_screen != NO_ERROR) return draw_screen;

  draw_screen = DrawMonthlyLog(app);
  if (draw_screen != NO_ERROR) return draw_screen;

  draw_screen = ShowStatusBar(app);
  if (draw_screen != NO_ERROR) return draw_screen;

  draw_screen = ShowKeyPage(app);
  if (draw_screen != NO_ERROR) return draw_screen;

  draw_screen = ShowIndexPage(app);
  if (draw_screen != NO_ERROR) return draw_screen;

  refresh();
  return NO_ERROR;
}

/* Show the status bar */
ErrorCode ShowStatusBar(AppData *app) {
  if (!app->show_status_bar) return NO_ERROR;

  Window *window = app->main_window;
  const int start_x = window->start_x;
  const int start_y = window->start_y;
  const int end_width = window->width + start_x;
  const int end_height = window->height + start_y - 1;

  SetColor(COLOR_BLACK, COLOR_BLACK, A_BOLD);
  for (int i = start_x; i < end_width; i++) mvaddch(end_height, i, ' ');
  if (strcmp(ICONS, "nerdicons") == 0) {
    int mode_length = 15;
    if (app->input_mode == NORMAL) {
      SetColor(COLOR_BLACK, COLOR_CYAN, A_BOLD);
      mvprintw(end_height, start_x, " 󰎐 NORMAL MODE ");
    } else if (app->input_mode == INSERT) {
      SetColor(COLOR_BLACK, COLOR_MAGENTA, A_BOLD);
      mvprintw(end_height, start_x, " 󰼭 INSERT MODE ");
    } else if (app->input_mode == COMMAND) {
      SetColor(COLOR_BLACK, COLOR_GREEN, A_BOLD);
      mvprintw(end_height, start_x, "  COMMAND MODE ");
      mode_length += 1;
    }
    SetColor(COLOR_BLACK, COLOR_YELLOW, A_BOLD);
    mvprintw(end_height, start_x + mode_length, " 󰥔 %02d:%02d ",
             app->current_hour, app->current_minute);

    SetColor(COLOR_BLACK, COLOR_GREEN, A_BOLD);
    const char *current_page[] = {" 󱕴 KEY ", " 󰉹 INDEX ",
                                  " 󰹢 FUTURE LOG ", " 󰸗 MONTLY LOG "};
    int page_title_length = 0;
    switch (app->page_history[0]) {
      case FUTURE_LOG:
        page_title_length = strlen(current_page[2]) - 3;
        mvprintw(end_height, end_width - page_title_length, "%s",
                 current_page[2]);
        break;
      case MONTHLY_LOG:
        page_title_length = strlen(current_page[3]) - 3;
        mvprintw(end_height, end_width - page_title_length, "%s",
                 current_page[3]);
        break;
      case DAILY_LOG:
        break;
      default:
        break;
    }
    SetColor(COLOR_BLACK, COLOR_YELLOW, A_BOLD);
    if (app->show_key_page == 1)
      mvprintw(end_height, end_width - 7 - page_title_length, "%s",
               current_page[0]);
    else if (app->show_index_page == 1)
      mvprintw(end_height, end_width - 9 - page_title_length, "%s",
               current_page[1]);
  }

  SetColor(COLOR_WHITE, NO_COLOR, A_BOLD);
  if (app->input_mode == INSERT) {
    int split_position = app->cursor_x;
    mvprintw(end_height + 1, start_x, "%.*s", split_position,
             app->insert_buffer);

    SetColor(COLOR_WHITE, NO_COLOR, A_BOLD | A_BLINK);
    mvprintw(end_height + 1, start_x + app->cursor_x, "▏");

    SetColor(COLOR_WHITE, NO_COLOR, A_BOLD);
    mvprintw(end_height + 1, start_x + split_position + 1, "%s",
             app->insert_buffer + split_position);
  } else
    mvprintw(end_height + 1, start_x, "%s", app->insert_buffer);

  return NO_ERROR;
}

/* Show index of pages */
ErrorCode ShowIndexPage(AppData *app) {
  if (!app->show_index_page) return NO_ERROR;

  Window *window = app->floating_window;
  const int window_height = 20;
  const int window_width = 30;
  const int window_x = app->middle_x - (window_width / 2);
  const int window_y = app->middle_y - (window_height / 2);

  ErrorCode update_floating_window = CreateFloatingWindow(
    &window, window_x, window_y, window_width, window_height);
  if (update_floating_window != NO_ERROR) return WINDOW_CREATION_ERROR;

  const int end_width = window->width + window->start_x - 2;
  const int end_height = window->height + window->start_y - 2;
  const int start_x = window->start_x + 1;
  const int start_y = window->start_y + 1;

  if (strcmp(ICONS, "nerdicons") == 0) {
    SetColor(COLOR_BLACK, COLOR_WHITE, A_BOLD);
    mvprintw(start_y + 1, start_x + 2, "INDEX 󰉹 ");

    SetColor(COLOR_WHITE, NO_COLOR, A_BOLD);
    mvprintw(start_y, end_width - 2, "(1)");
    mvprintw(end_height, end_width - 1, "01");

    mvprintw(start_y + 2, start_x + 2, "00-(?) Key page");
    mvprintw(start_y + 3, start_x + 2, "01-    Index page");
    mvprintw(start_y + 4, start_x + 2, "02-(f) Future log");
  }

  return NO_ERROR;
}

/* Show the page of symbols */
ErrorCode ShowKeyPage(AppData *app) {
  if (!app->show_key_page) return NO_ERROR;

  Window *window = app->floating_window;
  const int window_height = 17;
  const int window_width = 24;
  const int window_x = app->middle_x - (window_width / 2);
  const int window_y = app->middle_y - (window_height / 2);

  ErrorCode update_floating_window = CreateFloatingWindow(
    &window, window_x, window_y, window_width, window_height);
  if (update_floating_window != NO_ERROR) return WINDOW_CREATION_ERROR;

  const int end_width = window->width + window->start_x - 2;
  const int end_height = window->height + window->start_y - 2;
  const int start_x = window->start_x + 1;
  const int start_y = window->start_y + 1;

  if (strcmp(ICONS, "nerdicons") == 0) {
    SetColor(COLOR_BLACK, COLOR_WHITE, A_BOLD);
    mvprintw(start_y + 1, start_x + 2, "KEY 󱕴 ");

    SetColor(COLOR_WHITE, NO_COLOR, A_BOLD);
    mvprintw(start_y, end_width - 2, "(?)");
    mvprintw(end_height, end_width - 1, "00");

    const char *tasks[] = {" Task", " Task complete", " Task migrated",
                           " Task scheduled"};
    const char *others[] = {" Note", " Event", " Appointment"};
    const char *modifiers[] = {"󰈅  Priority", "󰓒  Idea"};

    for (size_t i = 0; i < sizeof(tasks) / sizeof(tasks[0]); ++i)
      mvprintw(start_y + 2 + i, start_x + 2, "%s", tasks[i]);

    for (size_t i = 0; i < sizeof(others) / sizeof(others[0]); ++i)
      mvprintw(start_y + 6 + i, start_x + 2, "%s", others[i]);

    for (size_t i = 0; i < sizeof(modifiers) / sizeof(modifiers[0]); ++i)
      mvprintw(start_y + 10 + i, start_x, "%s", modifiers[i]);

    SetColor(COLOR_BLACK, COLOR_RED, A_BOLD | A_UNDERLINE);
    mvprintw(start_y + 12, start_x + 2, " Cancelled task");
  }

  return NO_ERROR;
}

/* Display the future log page */
ErrorCode DrawFutureLog(AppData *app) {
  if (app->page_history[0] != FUTURE_LOG) return NO_ERROR;

  Window *window = app->main_window;
  CreateBorder(window, COLOR_WHITE, NO_COLOR, A_BOLD);
  if (window->width < MIN_WIDTH || window->height < MIN_HEIGTH) {
    DisplayTerminalSizeError(window);
    return NO_ERROR;
  }

  const int end_width = window->width + window->start_x - 2;
  const int end_height = window->height + window->start_y - 2;
  const int start_y = window->start_y + 1;

  const int number_of_months = (window->width >= 139) ? 6 : 3;
  const int window_width =
    ((app->width - 1) % 2 == 0) ? app->width - 1 : app->width;
  const int space_between_months =
    (number_of_months == 6) ? (window_width) / 6 : (window_width) / 3;

  if (strcmp(ICONS, "nerdicons") == 0) {
    SetColor(COLOR_WHITE, NO_COLOR, A_BOLD);
    mvprintw(start_y, end_width - 2, "(f)");
    if (number_of_months == 3)
      mvprintw(end_height, end_width - 4, "%02d-%02d", 2,
               app->current_future_log + 1);
    else
      mvprintw(end_height, end_width - 4, "%02d-%02d", 2,
               (app->current_future_log / 2) + 1);

    int start_month = 0;
    if (number_of_months == 6)
      start_month = (app->current_future_log * 3 >= 6) ? 6 : 0;
    else
      start_month = app->current_future_log * 3;
    ErrorCode render_month = NO_ERROR;
    for (int i = 0; i < number_of_months; i++) {
      int month_index = (start_month + i) % 12;
      int divider_x = i * space_between_months;
      if (i != 0)
        DrawVerticalLine(start_y, end_height, divider_x, i, number_of_months);

      int month_x = (((i * 2 + 1) * space_between_months) / 2);
      render_month =
        RenderMonth(app, month_index + 1, month_x - 9, start_y + 1);
      if (render_month != NO_ERROR) return render_month;
    }
  }

  return NO_ERROR;
}

/* Display the speficied month at the speficied location */
ErrorCode RenderMonth(AppData *app, int month, int start_x, int start_y) {
  if (month < 1 || month > 12) return INVALID_MONTH;

  time_t current_time = time(NULL);
  struct tm *time_info = localtime(&current_time);
  time_info->tm_mday = 1;
  time_info->tm_mon = month - 1;
  mktime(time_info);

  const char *month_names[] = {"January",   "February", "March",    "April",
                               "May",       "June",     "July",     "August",
                               "September", "October",  "November", "December"};
  const char *week_days = "Su Mo Tu We Th Fr Sa";

  int x = start_x;
  int y = start_y;
  SetColor(COLOR_WHITE, NO_COLOR, A_BOLD);

  int monthCenter = (20 - (int)(strlen(month_names[month - 1]) + 5)) / 2;
  mvprintw(y, x + monthCenter, "%s %4d", month_names[month - 1],
           app->current_year);

  y += 1;
  mvprintw(y, x, "%s", week_days);

  y += 1;
  for (int i = time_info->tm_wday; i > 0; i--) x += 3;
  while (time_info->tm_mon == month - 1) {
    SetColor(
      (time_info->tm_mday == app->current_day && month == app->current_month)
        ? COLOR_BLACK
        : COLOR_WHITE,
      (time_info->tm_mday == app->current_day && month == app->current_month)
        ? COLOR_CYAN
        : NO_COLOR,
      A_BOLD);

    mvprintw(y, x, "%2d", time_info->tm_mday);
    x += 3;

    time_info->tm_mday++;
    mktime(time_info);

    if (time_info->tm_wday == 0 && time_info->tm_mday != 1) {
      x = start_x;
      y += 1;
    }
  }

  return NO_ERROR;
}

/* Display the future log page */
ErrorCode DrawMonthlyLog(AppData *app) {
  if (app->page_history[0] != MONTHLY_LOG) return NO_ERROR;

  Window *window = app->main_window;
  CreateBorder(window, COLOR_WHITE, NO_COLOR, A_BOLD);
  if (window->width < MIN_WIDTH || window->height < MIN_HEIGTH) {
    DisplayTerminalSizeError(window);
    return NO_ERROR;
  }

  const int end_width = window->width + window->start_x - 2;
  const int end_height = window->height + window->start_y - 2;
  const int middle_x = window->middle_x;
  const int start_y = window->start_y + 1;
  const int start_x = window->start_x + 1;

  time_t current_time = time(NULL);
  struct tm *time_info = localtime(&current_time);
  time_info->tm_mday = 1;
  time_info->tm_mon = app->current_month - 1;
  mktime(time_info);

  if (strcmp(ICONS, "nerdicons") == 0) {
    SetColor(COLOR_WHITE, NO_COLOR, A_BOLD);
    mvprintw(start_y, end_width - 2, "(m)");
    mvprintw(end_height, end_width - 1, "%02d", 3);

    ErrorCode render_days =
      RenderDays(app, start_x + 2, start_y + 1, time_info->tm_wday);
    if (render_days != NO_ERROR) return render_days;

    DrawVerticalLine(start_y, end_height, middle_x, 0, 1);
  }

  return NO_ERROR;
}

/* Display the days of the current month in a vertical line
 * at the speficied location */
ErrorCode RenderDays(AppData *app, int start_x, int start_y, int day_of_week) {
  if (day_of_week < 0 || day_of_week > 6) return INVALID_DAY;
  const char *week_days[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

  for (int i = 1; i <= app->days_in_month; i++) {
    SetColor((i == app->current_day) ? COLOR_BLACK : COLOR_WHITE,
             (i == app->current_day) ? COLOR_CYAN : NO_COLOR, A_BOLD);
    mvprintw(start_y + i - 1, start_x, "%02d %s", i,
             week_days[(day_of_week + i - 1) % 7]);
  }
  return NO_ERROR;
}

/* Display terminal size error */
void DisplayTerminalSizeError(Window *window) {
  SetColor(COLOR_BLACK, COLOR_RED, A_BOLD);
  const int center_x = (window->width + window->start_x) / 2;
  const int center_y = (window->height + window->start_y) / 2;

  mvprintw(center_y - 2, center_x - 12, "TERMINAL SIZE TOO SMALL!");
  SetColor(COLOR_WHITE, NO_COLOR, A_BOLD);
  mvprintw(center_y - 1, center_x - 11, "Width = %2d Height = %2d",
           window->width, window->height);
  SetColor(COLOR_BLACK, COLOR_WHITE, A_BOLD);
  mvprintw(center_y + 1, center_x - 15, "SIZE NEEDED IN CURRENT CONFIG");
  SetColor(COLOR_WHITE, NO_COLOR, A_BOLD);
  mvprintw(center_y + 2, center_x - 11, "Width = %2d Height = %2d", MIN_WIDTH,
           MIN_HEIGTH);
}

/* Display vertical line */
void DrawVerticalLine(int start_y, int end_y, int x, int current_index,
                      int total_count) {
  if (current_index != total_count) {
    mvaddch(start_y - 1, x, ACS_TTEE);
    for (int j = start_y; j < end_y + 1; j++) mvaddch(j, x, ACS_VLINE);
  }
}
