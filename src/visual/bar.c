// A module that implements bar.h's definitions.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bar.h"
#include "colors.h"

// ========================== Progress Bar Struct ========================== //
ProgressBar* progress_bar_new(int pb_width, float pb_percent, char* color)
{
    // adjust inputs, if needed
    if (pb_width < PROGBAR_MIN_WIDTH) { pb_width = PROGBAR_MIN_WIDTH; }
    if (pb_percent < 0.0) { pb_percent = 0.0; }
    if (pb_percent > 1.0) { pb_percent = 1.0; }

    // attempt to allocate a progress bar
    ProgressBar* bar = calloc(1, sizeof(ProgressBar));
    if (!bar) { return NULL; }

    // fill in the struct fields
    bar->width = pb_width;
    bar->percentage = pb_percent;

    // attempt to find a matching color
    const char* cstr = color_from_name(color);
    if (cstr)
    { snprintf(bar->color, COLOR_MAX_LENGTH, "%s", cstr); }
    else
    { snprintf(bar->color, COLOR_MAX_LENGTH, "%s", C_BAR); }
    return bar;
}

void progress_bar_free(ProgressBar* bar)
{
    // if we're given a NULL pointer, return
    if (!bar) { return; }

    // otherwise, free the pointer
    free(bar);
}

char* progress_bar_to_string(ProgressBar* bar)
{
    // if we're given a NULL pointer, return NULL
    if (!bar) { return NULL; }

    // allocate a new string of the appropriate length
    char* result = calloc((bar->width * PROGBAR_CHARACTER_SIZE) +
                          (strlen(C_BAR_FRAME) * 2) +
                          strlen(bar->color) +
                          strlen(C_NONE),
                          sizeof(char));
    if (!result) { return NULL; }

    // --------------- Percentage String --------------- //
    int percent_string_length = 4;
    char percent_string[percent_string_length + 1];
    percent_string[percent_string_length] = '\0';
    snprintf(percent_string, percent_string_length + 1, "%-3d%%", (int) (bar->percentage * 100));
    // copy the local string to the result string
    snprintf(result, percent_string_length + 1, "%s", percent_string);

    // -------------- Progress Bar String -------------- //
    // allocate the local string
    int bar_string_length = (bar->width - percent_string_length) * PROGBAR_CHARACTER_SIZE + 1 +
                            (strlen(C_BAR_FRAME) * 2) + strlen(bar->color) + strlen(C_NONE);
    char bar_string[bar_string_length + 1];
    bar_string[bar_string_length] = '\0';
    // copy the left border
    int length = snprintf(bar_string, 2 + PROGBAR_CHARACTER_SIZE +
                          strlen(C_BAR_FRAME) + strlen(bar->color),
                          C_BAR_FRAME " %s%s", PROGBAR_L_BORDER, bar->color);
    // calculate the number of 'filled' slots vs the number of 'empty' slots
    int total_slots = bar->width - percent_string_length - 3;
    int filled_count = (int) ((float) total_slots * bar->percentage);
    // iterate and append bar characters
    for (int i = 0; i < total_slots; i++)
    {
        if (i < filled_count)
        {
            memmove(bar_string + length, PROGBAR_FILLED, PROGBAR_CHARACTER_SIZE);
            length += PROGBAR_CHARACTER_SIZE;
        }
        else
        {
            // switch to the bar frame's color when necessary
            if (i == filled_count)
            {
                memmove(bar_string + length, C_BAR_FRAME, strlen(C_BAR_FRAME));
                length += strlen(C_BAR_FRAME);
            }

            memmove(bar_string + length, PROGBAR_EMPTY, PROGBAR_CHARACTER_SIZE);
            length += PROGBAR_CHARACTER_SIZE;
        }
    }
    // copy the right border
    memmove(bar_string + length, PROGBAR_R_BORDER, PROGBAR_CHARACTER_SIZE);
    length += PROGBAR_CHARACTER_SIZE;
    memmove(bar_string + length, C_NONE, strlen(C_NONE));

    // copy the bar string onto the end string, and return it
    memmove(result + strlen(result), bar_string, length);
    return result;
}
