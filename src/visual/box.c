// A module that implements box.h's definitions
//
//      Connor Shugg

// Module inclusions
#include <string.h>
#include "box.h"
#include "colors.h"


// ====================== Helper Function Prototypes ======================= //
char** make_box_lines(uint16_t width, uint16_t height, char* title, char* text);
void free_string_array_null_terminated(char** lines);
char** free_string_array(char** lines, int length);
void fill_box_line(char** line, int width, char* left_edge,
                   char* middle, char* right_edge);
int fill_box_line_with_title(char** line, int width, char* left_edge,
                             char* middle, char* right_edge, char* title);
int fill_box_line_with_text(char** line, int width, char* left_edge,
                            char* middle, char* right_edge, char* text);
char** split_text_by_newline(char* text);
int count_string_extended_unicode(char* text, int text_length);


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

void box_adjust_to_text(Box* box, int force_readjust)
{
    // if we received a NULL box, return
    if (!box) { return; }

    // if the box's text is an empty string (or NULL), return
    if (!box->text || strlen(box->text) == 0)
    { return; }

    char** lines = split_text_by_newline(box->text);
    if (!lines) { return; }

    // count the number of lines in the text and find the longest line's
    // length in the lines
    int line_count = 0;
    int longest_width = -1;
    char** current = lines;
    while (*current)
    {
        // update the longest length and increment the line counter
        int length = strlen(*current);
        if (length > longest_width) { longest_width = length; }
        line_count++;

        current++;
    }
    if (lines) { free_string_array_null_terminated(lines); }

    // if the current box size is too small, adjust
    int width_diff = box->width - 4 - longest_width;
    int height_diff = box->height - 2 - line_count;

    // update the box width
    if (width_diff < 0) { box->width = longest_width + 4; }
    else if (force_readjust) { box->width = longest_width + 4; }
    // update the box height
    if (height_diff < 0) { box->height = line_count + 2; }
    else if (force_readjust) { box->height = line_count + 2; }
}

int box_print(Box* box)
{
    // check for a NULL pointer
    if (!box) { return 1; }

    // invoke a helper function to generate an array of lines to print
    char** lines = make_box_lines(box->width, box->height, box->title, box->text);
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

char** box_to_lines(Box* box)
{
    // check for a NULL pointer
    if (!box) { return NULL; }

    // invoke a helper function to generate an array of lines to print
    char** lines = make_box_lines(box->width, box->height, box->title, box->text);
    if (!lines) { return NULL; }
    return lines;
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
char** make_box_lines(uint16_t width, uint16_t height, char* title, char* text)
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
    lines[0] = calloc(line_width + 
                      (strlen(C_BOX) * 2) +
                      (strlen(C_NONE) * 2) + 1,
                      sizeof(char));
    /* calloc check */ if (!lines[0])
    /* calloc check */ { return free_string_array(lines, 0); }
    // attempt to fill the top line with the title, if it's long enough. If it
    // fails, fill the line as normal (without the title)
    if (fill_box_line_with_title(&lines[0], width, BOX_TL_CORNER, BOX_H_LINE, BOX_TR_CORNER, title))
    { fill_box_line(&lines[0], width, BOX_TL_CORNER, BOX_H_LINE, BOX_TR_CORNER); }

    // -------- Creating middle lines -------- //
    // attempt to split the box's text into lines. If it fails, free and return
    int has_text = text != NULL;
    char** text_lines = NULL;
    char** current_text_line = NULL;
    if (has_text)
    {
        text_lines = split_text_by_newline(text);
        if (!text_lines) { return free_string_array(lines, 1); }
        current_text_line = text_lines;
    }
    // iterate through indexes 1..(height - 2) and create middle strings
    for (int i = 1; i < height - 1; i++)
    {
        lines[i] = calloc(line_width +
                          (strlen(C_BOX) * 2) +
                          (strlen(C_NONE) * 2) + 1,
                          sizeof(char));
        /* calloc check */ if (!lines[i])
        /* calloc check */ { return free_string_array(lines, i); }

        // if we have a line of text to use for this line, attempt to use it
        int text_fill_failed = 1;
        if (has_text && *current_text_line)
        {
            text_fill_failed = fill_box_line_with_text(&lines[i], width, BOX_V_LINE,
                                                       " ", BOX_V_LINE, *(current_text_line++));
        }
        // if the text-fill failed, fill the line as normal
        if (text_fill_failed)
        { fill_box_line(&lines[i], width, BOX_V_LINE, " ", BOX_V_LINE); }
    }
    // free the text lines
    if (has_text)
    { free_string_array_null_terminated(text_lines); }

    // --------- Creating last line ---------- //    
    // create the last line: it will have corners and horizontal lines
    lines[height - 1] = calloc(line_width +
                               strlen(C_BOX) +
                               strlen(C_NONE) + 1,
                               sizeof(char));
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
    // compute lengths
    int length = strlen(*line);
    int left_length = strlen(left_edge);
    int middle_length = strlen(middle);
    int right_length = strlen(right_edge);

    // write the left edge
    memmove(*line + length, C_BOX, strlen(C_BOX));
    length += strlen(C_BOX);
    memmove(*line + length, left_edge, left_length);
    length += left_length;

    // write the middle characters
    for (int i = 0; i < width - 2; i++)
    {
        memmove(*line + length, middle, middle_length);
        length += middle_length;
    }

    // write the right edge
    memmove(*line + length, right_edge, right_length);
    length += right_length;
    memmove(*line + length, C_NONE, strlen(C_NONE));
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

    // check for the correct length - if the title length is longer than the
    // available room, adjust the title length
    int title_length = strlen(title);
    int available_length = width - 6;
    int has_runoff = title_length > available_length;
    if (has_runoff) { title_length = available_length; }
    
    // create a local copy of the string and copy in the correct number
    // of characters
    char title_copy[title_length + 1];
    memset(title_copy, 0, title_length + 1);
    int title_copy_length = 0;
    if (has_runoff)
    {
        int runoff_length = strlen(BOX_TEXT_RUNOFF);
        // add a portion of the title
        memmove(title_copy, title, title_length - runoff_length);
        title_copy_length += title_length - runoff_length;
        // add the runoff text
        memmove(title_copy + title_copy_length, BOX_TEXT_RUNOFF, runoff_length);
        title_copy_length += runoff_length;
    }
    else
    {
        memmove(title_copy, title, title_length);
        title_copy_length += title_length;
    }
    
    // otherwise, fill the string
    int line_length = strlen(*line);
    line_length += snprintf(*line, (strlen(C_BOX) * 2) + strlen(left_edge) +
                            strlen(BOX_H_LINE) + strlen(C_NONE) + title_length + 3,
                            C_BOX "%s%s" C_NONE " %s " C_BOX,
                            left_edge, BOX_H_LINE, title_copy);
    int current_length = title_length + 4;

    // fill the remaining spots (except the last) with the middle string
    int middle_length = strlen(middle);
    for (int i = current_length; i < width - 1; i++)
    {
        memmove(*line + line_length, middle, middle_length);
        line_length += middle_length;
    }

    // fill the final spot with the right edge string
    memmove(*line + line_length, right_edge, strlen(right_edge));
    memmove(*line + line_length + strlen(right_edge), C_NONE, strlen(C_NONE));

    // success - return 0
    return 0;
}

// Helper function that works the same way as 'fill_box_line', but it attempts
// to add left-justified text to the string. If the width of the line isn't
// long enough to hold the entire text, as much text is written as possible.
// On failure to write, a non-zero value is returned. On success, 0 is returned
int fill_box_line_with_text(char** line, int width, char* left_edge,
                             char* middle, char* right_edge, char* text)
{
    // if the text is NULL, return 1
    if (!text) { return 1; }

    // count the number of extended unicode characters in the string
    int text_length = strlen(text);
    int text_colors_length = count_color_codes(text, text_length);
    int unicode_count = count_string_extended_unicode(text, text_length);
    // if extended unicode characters are present, we need to adjust the text
    // length. Each extended character takes 3 bytes
    if (unicode_count > 0)
    {
        width += 2 * unicode_count;
        *line = realloc(*line, (width * BOX_CHARACTER_SIZE) +
                               (strlen(C_BOX) * 2) +
                               (strlen(C_NONE) * 2));
    }

    // determine how much room there is for the text
    int runoff_length = strlen(BOX_TEXT_RUNOFF);
    int available_length = (width - 2) - runoff_length + 1;
    int copied_text_length = text_length;
    if (text_length - text_colors_length > available_length)
    { copied_text_length = available_length + text_colors_length; }

    // make a local copy of the text of the appropriate length
    char text_copy[copied_text_length + 1];
    snprintf(text_copy, copied_text_length + 1, "%s", text);
    // if we couldn't fit all the text, add the 'runoff indicator' to the end
    if (text_length - text_colors_length > available_length)
    {
        snprintf(text_copy + strlen(text_copy) - runoff_length,
                 runoff_length + 1, "%s", BOX_TEXT_RUNOFF);
        //snprintf(text_copy + copied_text_length - runoff_length, runoff_length + 1,
                 //"%s", BOX_TEXT_RUNOFF);
    }

    // write the left edge
    int length = strlen(*line);
    length += snprintf(*line + length,
                       strlen(left_edge) + 2 + strlen(C_BOX) + strlen(C_NONE),
                       C_BOX "%s " C_NONE, left_edge);

    // write the middle characters
    length += snprintf(*line + length, copied_text_length + 1, "%s", text_copy);
    int middle_length = strlen(middle);
    for (int i = 0; i < width - 4 - (copied_text_length - text_colors_length); i++)
    { length += snprintf(*line + length, middle_length + 1, "%s", middle); }

    // write the right edge
    length += snprintf(*line + length,
                       strlen(right_edge) + 2 + strlen(C_BOX) + strlen(C_NONE),
                       C_BOX " %s" C_NONE, right_edge);

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

    // if the text is an empty string, return a char** containing a single
    // empty string
    if (strlen(text) == 0)
    {
        char** result = calloc(2, sizeof(char*));
        *result = calloc(1, sizeof(char));
        return result;
    }

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
    snprintf(text_copy, text_length + 1, "%s", text);
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
        snprintf(lines[i], line_length + 1, "%s", line);

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

// Helper function that takes in a string and returns the number of extended
// unicode characters found in the string.
int count_string_extended_unicode(char* text, int text_length)
{
    // check for a NULL pointer
    if (!text) { return 0; }

    // by looking at each character's integer value, we can tell if extended
    // unicode character are being used by seeing negative integer values. We
    // can identify an extended unicode character by seeing the following:
    //      '-30 -X -Y'
    // where -30 indicate the presence of an extended character.
    char unicode_flag = (char) -30;
    // count the number of extended unicode characters in the string
    int unicode_count = 0;
    for (int i = 0; i < text_length; i ++)
    {
        if (text[i] == unicode_flag && i < text_length - 2)
        {
            unicode_count += ((int) text[i + 1] < 0) &&
                             ((int) text[i + 2] < 0);
        }
    }
    
    return unicode_count;
}

