// A header file defining the structs and functions to represent a ttydo task:
// a single objective or goal to reach.
//
//      Connor Shugg

#ifndef TASK_H
#define TASK_H

// Module inclusions
#include <inttypes.h>

// ========================= Constants and Macros ========================== //
#define TASK_TITLE_MAX_LENGTH 32    // the max number of characters in a title

// default fields: used when a task has no title or description
#define TASK_DEFAULT_TITLE "(no title)"
#define TASK_DEFAULT_DESCRIPTION "(no description)"

// ============================== Task Struct ============================== //
typedef struct _Task
{
    char* title;            // the title of the task
    char* description;      // the description of the task
    uint64_t id;            // unique task ID
    int is_complete;        // whether or not the task is finished
} Task;

// Constructor: dynamically allocates memory for a new 'Task' struct, and
// populates the 'title' and 'description' fields with the given parameters.
// The pointer is returned. If allocation fails, NULL is returned.
// NOTE: the given strings are copied, and the copies are stored in the struct.
// NOTE: if the given title is longer than TASK_TITLE_MAX_LENGTH, the string
// is truncated to hold the only TASK_TITLE_MAX_LENGTH characters.
Task* task_new(char* title, char* desc);

// Destructor: takes in a pointer to a 'Task' struct and attempts to free the
// struct's memory.
void task_free(Task* task);

// Takes in a Task struct and attempts to create a dynamically-allocates string
// that repesents the task. The returned pointer must be freed after it's used.
// On failure, NULL is returned.
char* task_to_string(Task* task);

#endif
