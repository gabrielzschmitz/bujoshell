#ifndef INPUT_H_
#define INPUT_H_

#include <ncurses.h>

#include "bujoshell.h"

/* Defining some ASCII Keys */
#define MAX_INPUT_LENGTH   84
#define MAX_INPUT_PER_LINE 42
#define BACKSPACE          127
#define ESC                27
#define ENTER              10
#define SPACE              ' '
#define TAB                9
#define CTRLC              3
#define CTRLP              16
#define CTRLX              24
#define CTRLR              18
#define CTRLF              6
#define CTRLW              23
#define CTRLT              20

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

/* Handle cursor movement at normal mode */
int HandleNormalCursor(AppData *app);

/* Handle cursor movement at insert mode */
int HandleInsertCursor(AppData *app);

/* Handle input at popups */
int HandlePopupInput(AppData *app);

/* Handle all entry inputting */
ErrorCode InputEntry(AppData *app);

/* Separate all the input entry into the correct variables */
ErrorCode SeparateEntryVariables(const char *input, char *type, char **text,
                                 int *dd, int *mm);

/* Return INVALID_INPUT if a function key is detected */
ErrorCode IgnoreFunctionKeys(AppData *app);

#endif /* INPUT_H_ */
