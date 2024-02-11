#ifndef INPUT_H_
#define INPUT_H_

#include <ncurses.h>

#include "bujoshell.h"

/* Defining some ASCII Keys */
#define BACKSPACE 127
#define ESC       27
#define ENTER     10
#define TAB       9
#define CTRLC     3
#define CTRLP     16
#define CTRLX     24
#define CTRLR     18
#define CTRLF     6
#define CTRLW     23
#define CTRLT     20

/* Handle user input and app state */
ErrorCode HandleInputs(AppData *app);

/* Handle user input at normal mode */
ErrorCode HandleNormalMode(AppData *app);

/* Handle user input at insert mode */
ErrorCode HandleInsertMode(AppData *app);

/* Handle user input at command mode */
ErrorCode HandleCommandMode(AppData *app);

/* Function to get the app->user_input and insert into a growing buffer */
ErrorCode InsertCharacter(AppData *app, char input);

/* Function to remove character at cursor and shrink the buffer */
ErrorCode RemoveCharacter(AppData *app);

/* Handle cursor movement */
int HandleCursor(AppData *app);

#endif /* INPUT_H_ */
