// Implements the function definitions in task.h.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "task.h"
#include "visual/colors.h"

// ================ Defines and Helper Function Prototypes ================= //
#define TASK_COMMA_SCRIBE_STRING "<COMMA>" // when commas appear in task text
uint64_t generate_task_id(char* title);
int count_substring(char* text, int length, char* substring);
int replace_substring(char** text, int length, char* substring, char* replacement);


// ============================== Task Struct ============================== //
Task* task_new(char* title, char* desc)
{
    // attempt to allocate, and return NULL on failure
    Task* task = calloc(1, sizeof(Task));
    if (!task) { return NULL; }

    // find the correct amount of chars to copy from the description
    int desc_length = 0;
    if (desc) { desc_length = strlen(desc); }
    if (desc_length > TASK_DESCRIPTION_MAX_LENGTH)
    { desc_length = TASK_DESCRIPTION_MAX_LENGTH; }

    // allocate a description string - free and return NULL on failure
    if (desc)
    {
        task->description = calloc(desc_length + 1, sizeof(char));
        if (!task->description)
        {
            free(task);
            return NULL;
        }
        // copy the description string
        snprintf(task->description, desc_length + 1, "%s", desc);
    }

    // find the correct amount of characters to copy from the title string
    int title_length = 0;
    if (title) { title_length = strlen(title); }
    if (title_length > TASK_TITLE_MAX_LENGTH)
    { title_length = TASK_TITLE_MAX_LENGTH; }
    
    // allocate a title string - free and return NULL on failure
    if (title)
    {
        task->title = calloc(title_length + 1, sizeof(char));
        if (!task->title)
        {
            if (task->description) { free(task->description); }
            free(task);
            return NULL;
        }
        // copy over the correct amount of characters into the title string
        snprintf(task->title, title_length + 1, "%s", title);
    }
    else { task->title = NULL; }

    // generate an ID for the task using the task description (string hashing)
    task->id = generate_task_id(task->description);    

    // set the default color for the task
    task_set_color(task, NULL);

    // return the Task
    return task;
}

void task_free(Task* task)
{
    // if the given pointer is NULL, go no further
    if (!task) { return; }

    // attempt to free the string fields
    if (task->title) { free(task->title); }
    if (task->description) { free(task->description); }
    
    // free the pointer itself
    free(task);
}

char* task_to_string(Task* task)
{
    // if the pointer is NULL, return NULL
    if (!task) { return NULL; }

    // calculate the lengths of the title and description
    int title_length = 0;
    if (task->title) { title_length = strlen(task->title); }
    int desc_length = 0;
    if (task->description) { desc_length = strlen(task->description); }

    // allocate a string of the appropriate size
    int pad = strlen(TASK_DEFAULT_TITLE) + strlen(TASK_DEFAULT_DESCRIPTION) +
              strlen(C_TASK_CBOX) + strlen(task->color) + (strlen(C_NONE) * 2) + 16;
    if (task->is_complete)
    { pad += strlen(C_TASK_CBOX) + strlen(C_TASK_CBOX_DONE); }
    char* result = calloc(title_length + desc_length + pad, sizeof(char));
    if (!result) { return NULL; }
    int result_length = 0;

    // add a "[ ]" or "[X]" to use for a 'task.is_complete' indicator
    if (task->is_complete)
    {
        result_length += snprintf(result,
                                  5 + (strlen(C_TASK_CBOX) * 2) + strlen(C_TASK_CBOX_DONE) + strlen(C_NONE),
                                  C_TASK_CBOX "["
                                  C_TASK_CBOX_DONE "X"
                                  C_TASK_CBOX "] " C_NONE);
    }
    else
    {
        result_length += snprintf(result,
                                  5 + strlen(C_TASK_CBOX) + strlen(C_NONE),
                                  C_TASK_CBOX "[ ] " C_NONE);
    }

    // copy the title in
    if (task->title)
    {
        result_length += snprintf(result + result_length,
                                  title_length + strlen(task->color) + strlen(C_NONE) + 3,
                                  "%s%s: " C_NONE, task->color, task->title);
    }
    else
    {
        result_length += snprintf(result + result_length, strlen(TASK_DEFAULT_TITLE) + 3,
                                  "%s: ", TASK_DEFAULT_TITLE);
    }

    // copy the description in
    if (task->description)
    {
        result_length += snprintf(result + result_length, desc_length + 1,
                                  "%s", task->description);
    }
    else
    {
        result_length += snprintf(result + result_length,
                                  strlen(TASK_DEFAULT_DESCRIPTION) + 1,
                                  "%s", TASK_DEFAULT_DESCRIPTION);
    }
    
    return result;
}

void task_set_color(Task* task, char* name)
{
    if (!task)
    { return; }

    // look for a color string matching the name
    const char* cstr = color_from_name(name);

    // copy the color string into the task struct
    if (cstr)
    { snprintf(task->color, COLOR_MAX_LENGTH, "%s", cstr); }
    else
    { snprintf(task->color, COLOR_MAX_LENGTH, "%s", C_TASK_TITLE); }
}


// =========================== Helper Functions ============================ //
// Takes in the description of a task and uses a string-folding hash algorithm
// to generate a unique ID for a task. On failure, 0 is returned.
// I found this algorithm here:
// http://research.cs.vt.edu/AVresearch/hashing/strings.php
uint64_t generate_task_id(char* description)
{
    // make a local copy of the string with extra room
    int filled_length = 0;
    int string_length = 64;
    int salt_length = 16;
    if (description) { string_length = strlen(description); }
    char local[string_length + salt_length + 1];
    memset(local, 0, string_length + salt_length + 1);
    // if we were given a string, copy it in
    if (description)
    {
        snprintf(local, string_length + 1, "%s", description);
        filled_length = string_length;
    }

    // use 'filled_length'  to fill the remaining characters with random
    // gibberish. This will act as either a salt onto the given string, or
    // a completely random string for when 'description' is NULL.
    static int task_id_random_seeder = 0;
    srand(time(NULL) + task_id_random_seeder++);
    for (int i = filled_length; i < string_length + salt_length; i++)
    { local[i] = (char) ((rand() % 96) + 32); }

    int length = strlen(local) / 4;
    uint64_t sum = 0;
    // iterate through the strings characters
    for (int i = 0; i < length; i++)
    {
        // get a substring of characters
        char sub[5];
        snprintf(sub, 5, "%s", local + (i * 4));

        uint64_t multiplier = 1;
        for (int j = 0; j < strlen(sub); j++)
        {
            sum += sub[j] * multiplier;
            multiplier *= 256;
        }
    }

    // for the remaining characters: perform the same action
    char sub[5];
    snprintf(sub, 5, "%s", local + (length * 4));
    uint64_t multiplier = 1;
    for (int i = 0; i < strlen(sub); i++)
    {
        sum += sub[i] * multiplier;
        multiplier *= 256;
    }

    // return the resulting integer
    return sum;
}

// Counts the number of substrings in the given text.
int count_substring(char* text, int length, char* substring)
{
    char* current = text;
    int sub_occurrences = 0;
    while (current && current < text + length)
    {
        // search for the substring, and increment if found
        current = strstr(current, substring);
        if (current)
        {
            sub_occurrences++;
            current++;
        }
    }
    return sub_occurrences;
}

// Helper function for scribe string generation that takes in some dynamically-
// -allocated text, its length, a substring, and a replacement string, and
// replaces all occurrences of 'substring' with 'replacement', reallocing if
// necessary. The new string length is returned.
int replace_substring(char** text, int length, char* substring, char* replacement)
{
    // get the length of the two string args
    int sub_length = strlen(substring);
    int rep_length = strlen(replacement);

    // count the occurrences of the substring in the string
    int sub_occurrences = count_substring(*text, length, substring);

    // if there are no occurrences, just return the same length
    if (sub_occurrences == 0)
    { return length; }
    
    // otherwise, make a local copy of the string
    char temp[length + 1];
    snprintf(temp, length + 1, "%s", *text);

    // use the amount of substrings to determine the new length of the string
    // (reallocate if necessary)
    int new_length = length + (sub_occurrences * (rep_length - sub_length));
    if (new_length > length)
    { *text = realloc(*text, (new_length + 1) * sizeof(char)); }

    // copy each character in from the temporary copy
    char* c1 = *text;
    char* c2 = temp;
    for (int i = 0; i < length && c1 < *text + new_length; i++)
    {
        // if the current location of the text contains the substring, we'll
        // instead write the replacement
        if (!strncmp(c2, substring, sub_length))
        {
            snprintf(c1, rep_length + 1, "%s", replacement);
            c1 += rep_length;
            c2 += sub_length;
            continue;
        }

        // otherwise, we'll just copy the current byte and move to the next
        *(c1++) = *(c2++);
    }
    *c1 = '\0';

    return new_length;
}


// ========================== File String Parsing ========================== //
char* task_get_scribe_string(Task* task)
{
    // check for a NULL pointer, or a NULL title
    if (!task) { return NULL; }

    // if no title is given, use the default string
    if (!task->title)
    {
        int length = strlen(TASK_DEFAULT_TITLE);
        task->title = calloc(length + 1, sizeof(char));
        snprintf(task->title, length + 1, "%s", TASK_DEFAULT_TITLE);
    }
    // if no description is given, use the default string
    if (!task->description)
    {
        int length = strlen(TASK_DEFAULT_DESCRIPTION);
        task->description = calloc(length + 1, sizeof(char));
        snprintf(task->description, length + 1, "%s", TASK_DEFAULT_DESCRIPTION);
    }

    // convert the task's ID to a string
    int id_string_max_length = 24;
    char id_string[id_string_max_length];
    snprintf(id_string, id_string_max_length, "%lu", task->id);

    // convert the task's 'is_complete' to a string
    int complete_string_max_length = 2;
    char complete_string[complete_string_max_length];
    snprintf(complete_string, complete_string_max_length, "%d",
            task->is_complete != 0);
    
    // convert the task's color to a name string
    int color_string_max_length = COLOR_NAME_MAX_LENGTH;
    char color_string[color_string_max_length];
    const char* color_name = color_to_name(task->color);
    snprintf(color_string, color_string_max_length, "%s", color_name);

    // calculate the lengths of strings
    int id_length = strlen(id_string);
    int complete_length = strlen(complete_string);
    int title_length = strlen(task->title);
    int desc_length = strlen(task->description);
    int color_length = strlen(color_string);

    // adjust the lengths of the title and description to fit their maximum
    // length bounds (TASK_TITLE_MAX_LENGTH, TASK_DESCRIPTION_MAX_LENGTH)
    if (task->title && title_length > TASK_TITLE_MAX_LENGTH)
    {
        title_length = TASK_TITLE_MAX_LENGTH;
        task->title = realloc(task->title, (title_length + 1) * sizeof(char));
        task->title[title_length] = '\0';
    }
    if (task->description && desc_length > TASK_DESCRIPTION_MAX_LENGTH)
    {
        desc_length = TASK_DESCRIPTION_MAX_LENGTH;
        task->description = realloc(task->description, (desc_length + 1) * sizeof(char));
        task->description[desc_length] = '\0';
    }

    // first, we'll replace the commas with our comma marker in the strings
    title_length = replace_substring(&task->title, title_length, ",",
                                     TASK_COMMA_SCRIBE_STRING);
    desc_length = replace_substring(&task->description, desc_length, ",",
                                    TASK_COMMA_SCRIBE_STRING);
    // compute the total length
    int total_length = id_length + complete_length + title_length +
                       desc_length + color_length;

    // allocate a new string
    int safety_pad = 16;
    char* result = calloc(total_length + safety_pad, sizeof(char));
    snprintf(result, total_length + safety_pad, "%s,%s,%s,%s,%s", id_string,
             complete_string, task->title, task->description, color_string);
    return result;
}

Task* task_new_from_scribe_string(char* string)
{
    // check for a NULL string pointer
    if (!string) { return NULL; }

    // first, count the number of <COMMA> markers in the text (we don't count
    // these as part of the enforced max lengths, since they're used to replace
    // commas entered by the user)
    int length = strlen(string);
    int comma_marker_count = count_substring(string, length, TASK_COMMA_SCRIBE_STRING);

    // This string is likely coming straight from a file. To be safe, we need
    // to impose a maximum length the string can have.
    int max_length = TASK_TITLE_MAX_LENGTH + TASK_DESCRIPTION_MAX_LENGTH + 32 +
                     (comma_marker_count * (strlen(TASK_COMMA_SCRIBE_STRING) - 1));
    if (length > max_length) { length = max_length; }

    // make a local copy of the string of the correct length
    char local[length + 1];
    snprintf(local, length + 1, "%s", string);
    
    // from here, we'll collect each comma-separated value, one at a time, to
    // build a new Task struct
    errno = 0;
    // ---------- PIECE 1: Task ID ---------- //
    char* id_string = strtok(local, ",");
    if (!id_string) { return NULL; }
    // attempt to extract the 64-bit integer
    char* end = NULL;
    uint64_t id = strtol(id_string, &end, 10);
    if (errno) { return NULL; }
    
    // ---------- PIECE 2: is_complete ---------- //
    char* complete_string = strtok(NULL, ",");
    if (!complete_string) { return NULL; }
    // attempt to extract the integer
    uint8_t is_complete = strtol(complete_string, &end, 10);
    if (errno) { return NULL; }
    
    // ---------- PIECE 3: title ---------- //
    // this may or may not be NULL, if the string given was too short.
    // if it IS NULL, we'll keep it, since we can initialize a Task with a
    // NULL title
    char* title = strtok(NULL, ",");
    if (title)
    {
        // make a heap-allocated copy and use it to replace any comma markers
        // with actual commas
        title = strdup(title);
        replace_substring(&title, strlen(title),
                          TASK_COMMA_SCRIBE_STRING, ",");
    }
    
    // ---------- PIECE 4: description ---------- //
    // the same goes for the description as it does for the title: if it's
    // NULL, we'll keep the NULL value.
    char* description = strtok(NULL, ",");
    if (description)
    {
        // make a heap-allocated copy and use it to replace any comma markers
        // with actual commas
        description = strdup(description);
        replace_substring(&description, strlen(description),
                          TASK_COMMA_SCRIBE_STRING, ",");
    }

    // ------------- PIECE 5: color ------------- //
    char* color = strtok(NULL, ",");

    // create a new Task* struct and free strings as necessary
    Task* result = task_new(title, description);
    if (color) { task_set_color(result, color); }
    if (title) { free(title); }
    if (description) { free(description); }

    // if creating the task failed, return NULL
    if (!result) { return NULL; }

    // override the ID field (in case it was generated differently) and set
    // the 'is_complete' field
    result->id = id;
    result->is_complete = is_complete;
    
    // printf("Found ID: %lu\n", result->id);
    // printf("Complete? %d\n", result->is_complete);
    // printf("Title:    '%s'\n", result->title);
    // printf("Desc:     '%s'\n\n", result->description);

    // char* str = task_to_string(result);
    // printf("Task created:%s\n", str);
    // if (str) { free(str); }

    return result;
}
