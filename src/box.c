// A module that implements box.h's definitions
//
//      Connor Shugg

// Module inclusions
#include <string.h>
#include "box.h"


// ====================== Helper Function Prototypes ======================= //
char** make_box_lines(uint16_t width, uint16_t height, char* title);
void free_string_array_null_terminated(char** lines);
char** free_string_array(char** lines, int length);
void fill_box_line(char** line, int width, char* left_edge,
                   char* middle, char* right_edge);
int fill_box_line_with_title(char** line, int width, char* left_edge,
                             char* middle, char* right_edge, char* title);
char** split_text_by_newline(char* text);

// ======= main ======= //
int main()
{
    printf("box test:\n");
    Box* b1 = box_new(64, 4, "[TITLE OF THE BOX]", "Line 1\nLine 2\nLine 3");
    box_print(b1);
    box_free(b1);

    char** chars = split_text_by_newline("hello\nthere\ndude\nwhat's\nup");
    char** x = chars;
    while (*x != NULL)
    { printf("Line: \"%s\"\n", *(x++)); }
    free_string_array_null_terminated(chars);

    return 0;    
}


// ============================== Box Struct =============================== //
Box* box_new(uint16_t b_width, uint16_t b_height, char* b_title, char* b_text)
{
    // if the width or height are lower then the minimum values, adjust them
    if (b_width < BOX_MIN_WIDTH) { b_width = BOX_MIN_WIDTH; }
    if (b_height < BOX_MIN_HEIGHT) { b_height = BOX_MIN_HEIGHT; }

    // allocate a new box, and return NULL on failure
    Box* box = calloc(1, sizeof(Box));
    if (!box) { return NULL; }

    // set integer fields
    box->width = b_width;
    box->height = b_height;

    // set string fields
    if (!b_title) { box->title = NULL; }
    else { box->title = strdup(b_title); }
    if (!b_text) { box->text = NULL; }
    else { box->text = strdup(b_text); }

    // set default fields for the text alignment
    box->align_horz = LEFT_TOP;
    box->align_vert = LEFT_TOP;

    // return the new box
    return box;
}

int box_free(Box* box)
{
    // if the given pointer is NULL, return 1
    if (!box) { return 1; }

    // free string fields
    if (box->title) { free(box->title); }
    if (box->text) { free(box->text); }

    // free the pointer itself and return 0
    free(box);
    return 0;
}

int box_print(Box* box)
{
    // check for a NULL pointer
    if (!box) { return 1; }

    // invoke a helper function to generate an array of lines to print
    char** lines = make_box_lines(box->width, box->height, box->title);
    if (!lines)
    {
        fprintf(stdout, "Error: could not print %dx%d box!\n",
                box->width, box->height);
        return 1;
    }

    // print each line to STDOUT (iterate until NULL is reached)
    for (int i = 0; i < box->height; i++)
    { fprintf(stdout, "%s\n", lines[i]); }

    // free the allocated memory
    free_string_array_null_terminated(lines);

    return 0;
}


// =========================== Helper Functions ============================ //
// Helper function that takes in a width and height, and generates a series of
// strings, each representing a single line in the box. The number of lines
// returned is equal to the 'height' parameter. A title string is also taken
// in - if it's not NULL, and the width of the box is long enough to print the
// title, the title will be printed on the top line of the box.
// If an error occurs, or allocation fails, NULL is returned.
// NOTE: if the width or height is below the minimum value (box.h's
// BOX_MIN_WIDTH/BOX_MIN_HEIGHT), NULL is returned.
char** make_box_lines(uint16_t width, uint16_t height, char* title)
{
    // if width or height is zero, modify it
    if (width < BOX_MIN_WIDTH || height < BOX_MIN_HEIGHT)
    { return NULL; }

    // allocate an array of strings to represent each line (the extra slot at
    // the end will be NULL.
    char** lines = calloc(height + 1, sizeof(char*));
    /* calloc check */ if (!lines) { return NULL; }
    lines[height] = NULL;
    int line_width = width * BOX_CHARACTER_SIZE;
    
    // --------- Creating first line --------- //
    // create the first line: it will have corners and horizontal lines
    lines[0] = calloc(line_width + 1, sizeof(char));
    /* calloc check */ if (!lines[0])
    /* calloc check */ { return free_string_array(lines, 0); }
    // attempt to fill the top line with the title, if it's long enough. If it
    // fails, fill the line as normal (without the title)
    if (fill_box_line_with_title(&lines[0], width, BOX_TL_CORNER, BOX_H_LINE, BOX_TR_CORNER, title))
    { fill_box_line(&lines[0], width, BOX_TL_CORNER, BOX_H_LINE, BOX_TR_CORNER); }

    // -------- Creating middle lines -------- //
    // iterate through indexes 1..(height - 2) and create middle strings
    for (int i = 1; i < height - 1; i++)
    {
        lines[i] = calloc(line_width + 1, sizeof(char));
        /* calloc check */ if (!lines[i])
        /* calloc check */ { return free_string_array(lines, i); }
        fill_box_line(&lines[i], width, BOX_V_LINE, " ", BOX_V_LINE);
    }

    // --------- Creating last line ---------- //    
    // create the last line: it will have corners and horizontal lines
    lines[height - 1] = calloc(line_width + 1, sizeof(char));
    /* calloc check */ if (!lines[height - 1])
    /* calloc check */ { return free_string_array(lines, height - 1); }
    fill_box_line(&lines[height - 1], width, BOX_BL_CORNER, BOX_H_LINE, BOX_BR_CORNER);

    return lines;
}

// Helper function for 'make_box_lines' that's used to build a single line for
// a box. The parameters are as follows:
// - line:      The pointer to the string to be filled (must be calloc'd already)
// - width:     The width of the box to be drawn
// - left_edge: The box-drawing character/string for the left side of the box
// - middle:    The middle character/string used to fill the middle of the line
// - right_edge: The box-drawing character/string for the right side of the box
// Upon completion, 'line' is filled appropriately.
void fill_box_line(char** line, int width, char* left_edge,
                   char* middle, char* right_edge)
{
    // write the left edge
    strncat(*line, left_edge, strlen(left_edge));
    // write the middle characters
    int middle_length = strlen(middle);
    for (int i = 0; i < width - 2; i++)
    { strncat(*line, middle, middle_length); }
    // write the right edge
    strncat(*line, right_edge, strlen(right_edge));
}

// Helper function that works the same way as 'fill_box_line', but it attempts
// to add a title to the left side of the line. If the width of the line isn't
// long enough to fit the line, nothing is written and a non-zero value is
// returned. Otherwise, 'line' is filled and 0 is returned.
int fill_box_line_with_title(char** line, int width, char* left_edge,
                             char* middle, char* right_edge, char* title)
{
    // if the title is NULL, return 1
    if (!title) { return 1; }

    // check for the correct length
    int title_length = strlen(title);
    if (width - 4 < title_length) { return 1; }

    // otherwise, fill the string
    strncat(*line, BOX_TL_CORNER, strlen(BOX_TL_CORNER));
    strncat(*line, BOX_H_LINE, strlen(BOX_H_LINE));
    strncat(*line, title, title_length);
    int current_length = title_length + 2;

    // fill the remaining spots (except the last) with the middle string
    int middle_length = strlen(middle);
    for (int i = current_length; i < width - 1; i++)
    { strncat(*line, middle, middle_length); }

    // fill the final spot with the right edge string
    strncat(*line, right_edge, strlen(right_edge));

    // success - return 0
    return 0;
}

// Takes in a single string and attempts to split it into multiple strings
// separated by "\n". Returns a dynamically-allocated array of dynamically-
// allocated strings on success, and NULL on failure. The array returned
// will be suffixed by a NULL entry, to indicate the end of the array.
char** split_text_by_newline(char* text)
{
    // check for a NULL string
    if (!text) { return NULL; }

    // iterate through the string and count the number of "\n" occurrences
    int text_length = strlen(text);
    int line_count = 0;
    for (int i = 0; i < text_length; i++)
    { line_count += text[i] == '\n'; }
    line_count++; // add one more to account for the last line
    
    // allocate an array of strings to hold each line
    char** lines = calloc(line_count + 1, sizeof(char*));
    /* calloc check */ if (!lines) { return NULL; }

    // make a local copy of the text
    char text_copy[text_length + 1];
    memset(text_copy, 0, text_length + 1);
    strncpy(text_copy, text, text_length);
    // repeatedly find the occurrences of each "\n" and use it to copy each
    // line into a new string
    char* current = text_copy;
    for (int i = 0; i < line_count; i++)
    {
        char* line = strtok(current, "\n");

        // allocate a new string in the array
        int line_length = strlen(line);
        lines[i] = calloc(line_length + 1, sizeof(char));
        /* calloc check */ if (!lines[0])
        /* calloc check */ { return free_string_array(lines, i); }
        strncpy(lines[i], line, line_length);

        // update the pointer to NULL for all future calls of strtok()
        current = NULL;
    }

    return lines;
}


// Takes in a char** returned from and attempts to free each inner string and
// the string array itself.
void free_string_array_null_terminated(char** lines)
{
    // iterate until NULL is reached (there should be an extra slot at the end
    // of the array that's been NULL'd out)
    char** current = lines;
    while (*current)
    { free(*(current++)); }

    // free the array pointer
    free(lines);
}


// Takes in a char** and a length, and free 'length' inner string, and the
// string array pointer itself. Returns NULL.
char** free_string_array(char** lines, int length)
{
    // free each individual lines
    for (int i = 0; i < length; i++)
    { free(lines[i]); }
    // free the array itself
    free(lines);

    // return NULL
    return NULL;
}
