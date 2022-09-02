// This header file defines colors used when drawing ttydo on the command line.
//
//      Connor Shugg

#ifndef COLORS_H
#define COLORS_H

// Color definitions
#define C_NONE "\033[0m"                        // no color
#define C_BOX "\033[90m"                        // box border color
#define C_TASK_CBOX "\033[38;2;100;100;100m"    // task checkbox border color
#define C_TASK_CBOX_DONE "\033[36m"             // task checkbox "X" indicator
#define C_TASK_TITLE "\033[38;2;255;255;255m"   // task's title
#define C_BAR "\033[36m"                        // progress bar
#define C_BAR_FRAME "\033[38;2;81;81;81m"       // progress bar frame

// Other definitions
#define COLORS_MAX 32                           // max number of allowed colors

// =============================== Prototypes =============================== //
// Helper function that takes in a string and returns the number of bytes that
// are taken up by color escape sequence strings.
int count_color_codes(char* text, int text_length);

#endif
