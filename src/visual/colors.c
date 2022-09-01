// This header file defines colors used when drawing ttydo on the command line.
//
//      Connor Shugg

// Module includes
#include <stdlib.h>
#include <string.h>
#include "colors.h"

// Global array of colors.
const char* colors[] = {
    C_NONE,
    C_BOX,
    C_TASK_CBOX,
    C_TASK_CBOX_DONE,
    C_BAR,
    C_BAR_FRAME
};
const int colors_len = 6;

int count_color_codes(char* text, int text_length)
{
    int total = 0;

    // check for the presence of each color
    for (int i = 0; i < colors_len; i++)
    {
        // search the text for the substring, incrementing our total as we go
        char* tmp = text;
        while ((tmp = strstr(tmp, colors[i])))
        {
            total += strlen(colors[i]);
            tmp++;
        }
    }

    return total;
}

