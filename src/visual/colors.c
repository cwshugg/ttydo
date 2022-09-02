// This header file defines colors used when drawing ttydo on the command line.
//
//      Connor Shugg

// Module includes
#include <stdlib.h>
#include <string.h>
#include "colors.h"

// File-scope global array of colors.
static const char* colors[] = {
    C_NONE,
    C_BOX,
    C_TASK_CBOX,
    C_TASK_CBOX_DONE,
    C_TASK_TITLE,
    C_BAR,
    C_BAR_FRAME
};
static const int colors_len = 7;

// File-scope global defining *unique* color strings. Used for searching strings
// for colors.
static const char* colors_unique[COLORS_MAX];
static int colors_unique_len = 0;


// ================================ Helpers ================================= //
// Helper function that initializes anything needed for color enablement in
// ttydo.
static void colors_init()
{
    // use a static integer to determine if we've already initialized colors
    static int colors_init = 0;
    if (colors_init++)
    { return; }

    // iterate through the main color array and count unique colors
    for (int i = 0; i < colors_len; i++)
    {
        // for the current color, compare it against all remaining colors
        int duplicates = 0;
        for (int j = i + 1; j < colors_len; j++)
        { duplicates += !strcmp(colors[i], colors[j]); }

        // if there's at least one duplicate, we won't add this color to the
        // unique array. If there are none, it's safe to add
        if (duplicates == 0)
        { colors_unique[colors_unique_len++] = colors[i]; }
    }
}


// ======================= Prototype Implementations ======================== //
// See description in colors.h.
int count_color_codes(char* text, int text_length)
{
    colors_init();
    int total = 0;

    // check for the presence of each color
    for (int i = 0; i < colors_unique_len; i++)
    {
        // search the text for the substring, incrementing our total as we go
        char* tmp = text;
        while ((tmp = strstr(tmp, colors_unique[i])))
        {
            total += strlen(colors_unique[i]);
            tmp++;
        }
    }

    return total;
}

