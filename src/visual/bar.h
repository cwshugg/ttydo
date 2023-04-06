// A header file that defines the data structure and functions used to
// represent a progress bar.
//
//      Connor Shugg

#ifndef BAR_H
#define BAR_H

#include "colors.h"

// ========================= Constants and Macros ========================== //
// Progress bar drawing definitions
// https://www.utf8-chartable.de/unicode-utf8-table.pl?start=9472
// progress bar blocks
#define PROGBAR_FILLED "\u2588"     // for a filled progress bar block
#define PROGBAR_EMPTY "\u2500"      // for an empty progress bar block
// bar edges
#define PROGBAR_L_BORDER "\u251c"   // the left border of the progress bar
#define PROGBAR_R_BORDER "\u2524"   // the right border of the progress bar
// other
#define PROGBAR_CHARACTER_SIZE 3    // number of bytes that make up each char
#define PROGBAR_MIN_WIDTH 8         // minimum number of characters


// ========================== Progress Bar Struct ========================== //
typedef struct _ProgressBar
{
    int width;          // the total width of the printed bar/percentage
    float percentage;   // the percent the progress bar is full
    char color[COLOR_MAX_LENGTH]; // the progress bar's color
} ProgressBar;

// Takes in a width and a percentage and attempts to allocate a new ProgressBar
// object. The dynamically-allocated pointer is returned. On failure, NULL is
// returned. If too short of a width is given, it's instead set to
// PROGBAR_MIN_WIDTH, and if an invalid percentage is given, it's set to 0.0 or
// 1.0. The percentage is expected as a value between 0.0 and 1.0.
ProgressBar* progress_bar_new(int pb_width, float pb_percent, char* color);

// Takes in a ProgressBar pointer and attempts to free the memory associated
// with the progress bar.
void progress_bar_free(ProgressBar* bar);

// Takes in a progress bar pointer and attempts to convert it to a string
// portraying a visual progress bar. The string is dynamically allocated and a
// pointer to it is returned. On failure, NULL is returned.
char* progress_bar_to_string(ProgressBar* bar);

#endif
