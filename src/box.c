// A module that implements box.h's definitions
//
//      Connor Shugg

// Module inclusions
#include <string.h>
#include "box.h"


// ====================== Helper Function Prototypes ======================= //
char** make_box_lines(uint16_t width, uint16_t height);
void free_box_lines(char** lines);
char** abort_and_free_box_lines(char** lines, int length);
void fill_box_line(char** line, int width, char* left_edge,
                   char* middle, char* right_edge);


// ======= main ======= //
int main()
{

    printf("box test:\n");
    Box* b1 = box_new(80, 8, "Title Test", "This is the box's text.");
    box_print(b1);

    return 0;    
}


// ============================== Box Struct =============================== //
Box* box_new(uint16_t b_width, uint16_t b_height, char* b_title, char* b_text)
{
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
    char** lines = make_box_lines(box->width, box->height);
    if (!lines) { return 1; }

    // print each line to STDOUT
    for (int i = 0; i < box->height; i++)
    { fprintf(stdout, "%s\n", lines[i]); }

    return 0;
}


// =========================== Helper Functions ============================ //
// Helper function that takes in a width and height, and generates a series of
// strings, each representing a single line in the box. The number of lines
// returned is equal to the 'height' parameter.
// If an error occurs, or allocation fails, NULL is returned.
char** make_box_lines(uint16_t width, uint16_t height)
{
    // allocate an array of strings to represent each line (the extra slot at
    // the end will be NULL.
    char** lines = calloc(height + 1, sizeof(char*));
    /* calloc check */ if (!lines)
    /* calloc check */ { return NULL; }
    lines[height] = NULL;
    int line_width = width * BOX_CHARACTER_SIZE;
    
    // --------- Creating first line --------- //
    // create the first line: it will have corners and horizontal lines
    lines[0] = calloc(line_width + 1, sizeof(char));
    /* calloc check */ if (!lines[0])
    /* calloc check */ { return abort_and_free_box_lines(lines, 0); }
    fill_box_line(&lines[0], width, BOX_TL_CORNER, BOX_H_LINE, BOX_TR_CORNER);

    // -------- Creating middle lines -------- //
    // iterate through indexes 1..(height - 2) and create middle strings
    for (int i = 1; i < height - 1; i++)
    {
        lines[i] = calloc(line_width + 1, sizeof(char));
        /* calloc check */ if (!lines[i])
        /* calloc check */ { return abort_and_free_box_lines(lines, i); }
        fill_box_line(&lines[i], width, BOX_V_LINE, " ", BOX_V_LINE);
    }

    // --------- Creating last line ---------- //    
    // create the last line: it will have corners and horizontal lines
    lines[height - 1] = calloc(line_width + 1, sizeof(char));
    /* calloc check */ if (!lines[height - 1])
    /* calloc check */ { return abort_and_free_box_lines(lines, height - 1); }
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
    for (int i = 0; i < width - 2; i++)
    { strncat(*line, middle, strlen(middle)); }
    // write the right edge
    strncat(*line, right_edge, strlen(right_edge));
}

// Takes in a char** returned from 'make_box_lines' and attempts to free each
// inner string, and the string array itself.
void free_box_lines(char** lines)
{
    // iterate until NULL is reached (there should be an extra slot at the end
    // of the array that's been NULL'd out)
    char** current = lines;
    while (*current)
    { free(*(current++)); }

    // free the array pointer
    free(lines);
}


// Helper function for when an error occurs, and 'make_box_lines' must free all
// the memory it's allocated thus far. Accepts a partially-complete array of
// line strings, and 'length', specifying the number of lines allocated.
// Each is freed, and NULL is returned.
char** abort_and_free_box_lines(char** lines, int length)
{
    // free each individual lines
    for (int i = 0; i < length; i++)
    { free(lines[i]); }
    // free the array itself
    free(lines);

    // return NULL
    return NULL;
}

