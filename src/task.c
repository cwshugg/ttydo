// Implements the function definitions in task.h.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "task.h"

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
    char* result = calloc(title_length + desc_length + 8, sizeof(char));
    if (!result) { return NULL; }

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


// ================================ Testing ================================ //
#include "box.h"
int main()
{
    Task* t1 = task_new("Task 1's Title", "Task 1's Description");
    char* t1_string = task_to_string(t1);
    printf("task with title and description:\n%s\n", t1_string);
    free(t1_string);

    Box* t1_box = box_new(0, 0, t1->title, t1->description);
    box_adjust_to_text(t1_box, 0);
    box_print(t1_box);

    box_free(t1_box);
    task_free(t1);

    return 0;
}
