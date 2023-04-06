// This header file defines colors used when drawing ttydo on the command line.
//
//      Connor Shugg

#ifndef COLORS_H
#define COLORS_H

// Box/default colors
#define C_NONE "\033[0m"                        // no color
#define C_BOX "\033[90m"                        // box border color
#define C_TASK_CBOX "\033[38;2;100;100;100m"    // task checkbox border color
#define C_TASK_CBOX_DONE "\033[36m"             // task checkbox "X" indicator
#define C_TASK_TITLE "\033[38;2;255;255;255m"   // task's title
#define C_BAR "\033[38;2;90;90;90m"             // progress bar
#define C_BAR_FRAME "\033[38;2;81;81;81m"       // progress bar frame

// Custom colors
#define C_WHITE "\033[38;2;255;255;255m"
#define C_MUD "\033[38;2;100;60;25m"
#define C_CLAY "\033[38;2;150;75;50m"
#define C_MAROON "\033[38;2;150;50;75m"
#define C_CRIMSON "\033[38;2;200;50;50m"
#define C_RED "\033[38;2;255;25;25m"
#define C_MAGMA "\033[38;2;240;90;0m"
#define C_ORANGE "\033[38;2;255;125;0m"
#define C_BRASS "\033[38;2;200;150;0m"
#define C_GOLD "\033[38;2;250;200;50m"
#define C_YELLOW "\033[38;2;225;200;0m"
#define C_LEMON "\033[38;2;255;255;0m"
#define C_LIME "\033[38;2;175;225;0m"
#define C_ACID "\033[38;2;150;255;0m"
#define C_GREEN "\033[38;2;50;200;50m"
#define C_FOREST "\033[38;2;50;150;50m"
#define C_TEAL "\033[38;2;50;175;125m"
#define C_CYAN "\033[38;2;50;200;200m"
#define C_SEA "\033[38;2;125;225;225m"
#define C_SKY "\033[38;2;150;200;255m"
#define C_RAIN "\033[38;2;100;175;255m"
#define C_BLUE "\033[38;2;50;150;255m"
#define C_INDIGO "\033[38;2;50;100;200m"
#define C_VIOLET "\033[38;2;160;100;255m"
#define C_PURPLE "\033[38;2;175;50;225m"
#define C_LAVENDER "\033[38;2;215;175;255m"
#define C_PINK "\033[38;2;255;150;255m"

// Other definitions
#define COLORS_MAX 64                           // max number of allowed colors
#define COLOR_MAX_LENGTH 64                     // max length of color code
#define COLOR_NAME_MAX_LENGTH 64                // max length of color name

// =============================== Prototypes =============================== //
// Helper function that takes in a string and returns the number of bytes that
// are taken up by color escape sequence strings.
int count_color_codes(char* text, int text_length);

// Returns the number of available colors.
int color_count(void);

// Returns a color code given an index.
const char* color_from_index(int idx);

// Returns a color name given an index.
const char* color_name_from_index(int idx);

// Returns the color code for a named color.
const char* color_from_name(char* name);

// Returns the color name, given the color code.
const char* color_to_name(char* color);

#endif
