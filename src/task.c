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

    // if the description is NULL, set the task's description to NULL.
    // Otherwise, duplicate the description string
    if (!desc) { task->description = NULL; }
    else { task->description = strdup(desc); }

    // find the correct amount of characters to copy from the title string
    int title_length = 0;
    if (title) { title_length = strlen(title); }
    if (title_length > TASK_TITLE_MAX_LENGTH)
    { title_length = TASK_TITLE_MAX_LENGTH; }
    
    // allocate a title string - if allocation fails, free the already-alloc'd
    // memory and return NULL
    if (title)
    {
        task->title = calloc(title_length + 1, sizeof(char));
        if (!task->title)
        {
            free(task->description);
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
    char* result = calloc(title_length + desc_length + 16, sizeof(char));
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
    // if we were given a NULL pointer, we'll instead randomly generate an ID
    int used_random = 0;
    if (!description)
    {
        srand(time(NULL));
        // allocate a new string and point 'description' at it
        int string_length = 64;
        description = calloc(string_length + 1, sizeof(char));
        if (!description) { return 0; }
        // fill the string with random characters
        for (int i = 0; i < string_length + 1; i++)
        { description[i] = (char) ((rand() % 96) + 32); }

        // toggle the switch for later
        used_random = 1;
    }

    int length = strlen(description) / 4;
    uint64_t sum = 0;
    // iterate through the strings characters
    for (int i = 0; i < length; i++)
    {
        // get a substring of characters
        char sub[5] = {'\0'};
        strncpy(sub, description + (i * 4), 4);

        uint64_t multiplier = 1;
        for (int j = 0; j < strlen(sub); j++)
        {
            sum += sub[j] * multiplier;
            multiplier *= 256;
        }
    }

    // for the remaining characters: perform the same action
    char sub[5] = {'\0'};
    strncpy(sub, description + (length * 4), 4);
    uint64_t multiplier = 1;
    for (int i = 0; i < strlen(sub); i++)
    {
        sum += sub[i] * multiplier;
        multiplier *= 256;
    }

    // if we used a random string, free it
    if (used_random) { free(description); }

    // return the resulting integer
    return sum;
}
