// Implements the function definitions in task.h.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "task.h"

// ====================== Helper Function Prototypes ======================= //
uint64_t generate_task_id(char* title);


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
        strncpy(task->description, desc, desc_length);
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
        strncpy(task->title, title, title_length);
    }
    else { task->title = NULL; }

    // generate an ID for the task using the task description (string hashing)
    task->id = generate_task_id(task->description);    

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
    int pad = strlen(TASK_DEFAULT_TITLE) + strlen(TASK_DEFAULT_DESCRIPTION) + 16;
    char* result = calloc(title_length + desc_length + pad, sizeof(char));
    if (!result) { return NULL; }

    // add a "[ ]" or "[X]" to use for a 'task.is_complete' indicator
    if (task->is_complete)
    { strncat(result, "[X] ", 4); }
    else
    { strncat(result, "[ ] ", 4); }

    // copy the title in
    if (task->title)
    { strncat(result, task->title, title_length); }
    else
    { strncat(result, TASK_DEFAULT_TITLE, strlen(TASK_DEFAULT_TITLE)); }

    // copy a separator in
    strncat(result, ": ", 2);

    // copy the description in
    if (task->description)
    { strncat(result, task->description, desc_length); }
    else
    { strncat(result, TASK_DEFAULT_DESCRIPTION, strlen(TASK_DEFAULT_DESCRIPTION)); }
    
    return result;
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
        strncpy(local, description, string_length);
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
        char sub[5] = {'\0'};
        strncpy(sub, local + (i * 4), 4);

        uint64_t multiplier = 1;
        for (int j = 0; j < strlen(sub); j++)
        {
            sum += sub[j] * multiplier;
            multiplier *= 256;
        }
    }

    // for the remaining characters: perform the same action
    char sub[5] = {'\0'};
    strncpy(sub, local + (length * 4), 4);
    uint64_t multiplier = 1;
    for (int i = 0; i < strlen(sub); i++)
    {
        sum += sub[i] * multiplier;
        multiplier *= 256;
    }

    // return the resulting integer
    return sum;
}


// ========================== File String Parsing ========================== //
char* task_get_scribe_string(Task* task)
{
    // check for a NULL pointer, or a NULL title
    if (!task) { return NULL; }

    // if no title is given, use the default string
    char* title = task->title;
    if (!title) { title = TASK_DEFAULT_TITLE; }
    // if no description is given, use the default string
    char* desc = task->description;
    if (!desc) { desc = TASK_DEFAULT_DESCRIPTION; }

    // convert the task's ID to a string
    char id_string[24] = {'\0'};
    sprintf(id_string, "%lu", task->id);

    // convert the task's 'is_complete' to a string
    char complete_string[2] = {'\0'};
    sprintf(complete_string, "%d", task->is_complete != 0);

    // calculate the lengths of strings
    int id_length = strlen(id_string);
    int complete_length = strlen(complete_string);
    int title_length = strlen(title);
    int desc_length = strlen(desc);
    int total_length = id_length + complete_length + title_length + desc_length;

    // adjust the lengths of the title and description to fit their maximum
    // length bounds (TASK_TITLE_MAX_LENGTH, TASK_DESCRIPTION_MAX_LENGTH)
    if (task->title && title_length > TASK_TITLE_MAX_LENGTH)
    {
        title_length = TASK_TITLE_MAX_LENGTH;
        task->title = realloc(task->title, (title_length + 1) * sizeof(char));
        task->title[title_length] = '\0';
        title = task->title;
    }
    if (task->description && desc_length > TASK_DESCRIPTION_MAX_LENGTH)
    {
        desc_length = TASK_DESCRIPTION_MAX_LENGTH;
        task->description = realloc(task->description, (desc_length + 1) * sizeof(char));
        task->description[desc_length] = '\0';
        desc = task->description;
    }

    // allocate a new string
    int safety_pad = 16;
    char* result = calloc(total_length + safety_pad, sizeof(char));
    snprintf(result, total_length + safety_pad, "%s,%s,%s,%s", id_string,
             title, desc, complete_string);
    return result;
}

Task* task_new_from_scribe_string(char* string)
{
    // check for a NULL string pointer
    if
     (!string) { return NULL; }
    //int length = strlen(string);


    // count the number of commas in the string
    //for (int i =)
    return NULL;
}
