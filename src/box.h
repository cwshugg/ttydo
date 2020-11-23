// This header file defines functions used to draw boxes on the terminal.
// Functions are implemented in box.c.
//
//      Connor Shugg

// Module inclusions
#include<stdlib.h>
#include<stdio.h>
#include<inttypes.h>
#include<wchar.h>

// ======================== Box-Drawing Characters ========================= //
// Box-drawing character definitions
// https://www.daniweb.com/programming/software-development/code/476650/linux-console-ascii-box-drawing-characters
// https://www.utf8-chartable.de/unicode-utf8-table.pl?start=9472
// corners
#define BOX_BR_CORNER "\e(0\x6a\e(B"    // bottom-right corner
#define BOX_TR_CORNER "\e(0\x6b\e(B"    // top-right corner
#define BOX_BL_CORNER "\e(0\x6d\e(B"    // bottom-left corner
#define BOX_TL_CORNER "\e(0\x6c\e(B"    // top-left corner
// lines
#define BOX_H_LINE "\e(0\x71\e(B"       // horizontal line
#define BOX_V_LINE "\e(0\x78\e(B"       // vertical line
// crosses
#define BOX_M_CROSS "\e(0\x6e\e(B"      // middle cross
#define BOX_L_CROSS "\e(0\x74\e(B"      // left cross
#define BOX_R_CROSS "\e(0\x75\e(B"      // right cross
#define BOX_B_CROSS "\e(0\x76\e(B"      // bottom cross
#define BOX_T_CROSS "\e(0\x77\e(B"      // top cross
// other
#define BOX_CHARACTER_SIZE 8            // number of bytes for each character
#define BOX_MIN_WIDTH 2                 // the minimum width a box can be
#define BOX_MIN_HEIGHT 2                // the minimum height a box can be
// text-related constants
#define BOX_TEXT_RUNOFF "..."

// ============================== Box Struct =============================== //// The 'box' struct defines a data structure representing a box to be drawn on
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
