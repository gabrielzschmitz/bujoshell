#ifndef INIT_H_
#define INIT_H_

#include <ncurses.h>

#include "bujoshell.h"

/* Initialize screen with some little configs */
void InitScreen(void);

/* Initialize variables */
ErrorCode InitApp(AppData *app);

/* Initialize a future log */
void InitFutureLog(FutureLogData *future_log);

/* Initialize a window size */
Window *InitWindowSize(int start_x, int start_y, int width, int height);

/* Init all windows */
ErrorCode InitWindows(AppData *app);

/* End the app and screen */
void EndApp(AppData *app);

#endif /* INIT_H_ */
