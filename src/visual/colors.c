// This header file defines colors used when drawing ttydo on the command line.
//
//      Connor Shugg

// Module includes
#include <stdlib.h>
#include <string.h>
#include "colors.h"

// File-scope global array of colors.
static const char* colors[] = {
    // default colors
    C_NONE,
    C_BOX,
    C_TASK_CBOX,
    C_TASK_CBOX_DONE,
    C_TASK_TITLE,
    C_BAR,
    C_BAR_FRAME,

    // custom colors
    C_WHITE,
    C_MUD,
    C_CLAY,
    C_MAROON,
    C_CRIMSON,
    C_RED,
    C_MAGMA,
    C_ORANGE,
    C_BRASS,
    C_GOLD,
    C_YELLOW,
    C_LEMON,
    C_LIME,
    C_ACID,
    C_GREEN,
    C_FOREST,
    C_CYAN,
    C_SKY,
    C_BLUE,
    C_INDIGO,
    C_VIOLET,
    C_PURPLE,
    C_LAVENDER,
    C_PINK
};
static const char* color_names[] = {
    // default colors
    "none",
    "box",
    "taskcbox",
    "taskcboxdone",
    "tasktitle",
    "bar",
    "barframe",

    // custom colors
    "white",
    "mud",
    "clay",
    "maroon",
    "crimson",
    "red",
    "magma",
    "orange",
    "brass",
    "gold",
    "yellow",
    "lemon",
    "lime",
    "acid",
    "green",
    "forest",
    "cyan",
    "sky",
    "blue",
    "indigo",
    "violet",
    "purple",
    "lavender",
    "pink"
};
static const int colors_len = 31;

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

int color_count(void)
{
    return colors_len;
}

const char* color_from_index(int idx)
{
    if (idx < 0 || idx >= colors_len)
    { return NULL; }
    return colors[idx];
}

const char* color_name_from_index(int idx)
{
    if (idx < 0 || idx >= colors_len)
    { return NULL; }
    return color_names[idx];
}

// Returns a color name given an index.
const char* color_from_name(char* name)
{
    if (!name)
    { return NULL; }

    // iterate through the name list, searching for a match
    for (int i = 0; i < colors_len; i++)
    {
        if (!strncmp(name, color_names[i], strlen(name)))
        { return colors[i]; }
    }
    return NULL;
}

const char* color_to_name(char* color)
{
    if (!color)
    { return NULL; }

    for (int i = 0; i < colors_len; i++)
    {
        if (!strcmp(color, colors[i]))
        { return color_names[i]; }
    }
    return NULL;
}

