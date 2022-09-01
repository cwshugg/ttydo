// This header file defines functions used to draw boxes on the terminal.
// Functions are implemented in box.c.
//
//      Connor Shugg

#ifndef BOX_H
#define BOX_H

// Module inclusions
#include<stdlib.h>
#include<stdio.h>
#include<inttypes.h>

// ======================== Box-Drawing Characters ========================= //
// Box-drawing character definitions
// https://www.utf8-chartable.de/unicode-utf8-table.pl?start=9472
// corners
#define BOX_BR_CORNER "\u2518"          // bottom-right corner
#define BOX_TR_CORNER "\u2510"          // top-right corner
#define BOX_BL_CORNER "\u2514"          // bottom-left corner
#define BOX_TL_CORNER "\u250c"          // top-left corner
// lines
#define BOX_H_LINE "\u2500"             // horizontal line
#define BOX_V_LINE "\u2502"             // vertical line
// crosses
#define BOX_M_CROSS "\u253c"            // middle cross
#define BOX_L_CROSS "\u251c"            // left cross
#define BOX_R_CROSS "\u2524"            // right cross
#define BOX_B_CROSS "\u2534"            // bottom cross
#define BOX_T_CROSS "\u252c"            // top cross
// other
#define BOX_CHARACTER_SIZE 3            // number of bytes for each character
#define BOX_MIN_WIDTH 2                 // the minimum width a box can be
#define BOX_MIN_HEIGHT 2                // the minimum height a box can be
// text-related constants
#define BOX_TEXT_RUNOFF "..."

// ============================== Box Struct =============================== //
// The 'box' struct defines a data structure representing a box to be drawn on
// the terminal window.
typedef struct _Box
{
    uint16_t width;     // width, in characters, of the box
    uint16_t height;    // height, in characters, of the box
    char* title;        // a title displayed in the top-left  of the box
    char* text;         // text to be displayed inside of the box
} Box;

// Constructor: dynamically allocates a new 'Box' object and returns a pointer
// to it. Takes in:
// - b_width - the width of the box
// - b_height - the height of the box
// - b_title - the title of the box
// - b_text - the text inside the box
// The given char* pointers will have their values COPIED onto the heap in this
// function. On failure, NULL is returned.
// NOTE: if the width or height is below the minimum value (box.h's
// BOX_MIN_WIDTH/BOX_MIN_HEIGHT), they'll be set to the minimum values.
Box* box_new(uint16_t b_width, uint16_t b_height, char* b_title, char* b_text);

// Takes in a Box pointer and attempts to free it. Returns 0 on success, and
// a non-zero value on error.
int box_free(Box* box);

// Takes in a Box pointer and adjusts the width and height values to account
// for the length of the text, and the number of lines of text.
// The second 'force_readjust' parameter is used to force the box size to
// change - even if the box is alredy big enough hold the text.
void box_adjust_to_text(Box* box, int force_readjust);

// Takes in a Box pointer and attempts to draw it to the terminal. Returns 0 on
// success and a non-zero value on error.
// NOTE: it's best to ensure the width of the box is long enough to hold the
// title of the box (if it has one). Otherwise, the print may not look ideal.
int box_print(Box* box);

// Takes in a Box pointer and generates the lines needed to draw the box to the
// terminal. A pointer to a dynamically-allocated array of dynamically-allocated
// strings is returned. The number of dynamically-allocated strings in the
// array is equal to the height of the given box. The array is also NULL
// terminated.
char** box_to_lines(Box* box);

#endif
