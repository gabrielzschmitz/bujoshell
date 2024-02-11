#ifndef UPDATE_H_
#define UPDATE_H_

#include <ncurses.h>

#include "bujoshell.h"

/* Update variables */
ErrorCode UpdateApp(AppData *app);

/* Update a window size */
ErrorCode UpdateWindowSize(Window **win, int start_x, int start_y, int width,
                           int height);

/* Update current hour and minute */
ErrorCode UpdateTime(AppData *app);

/* Creates a floating window and empty it's content */
ErrorCode CreateFloatingWindow(Window **window, int x, int y, int width,
                               int height);

#endif /* UPDATE_H_ */
