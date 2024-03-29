// A header file defining the structs and functions to represent a ttydo task:
// a single objective or goal to reach.
//
//      Connor Shugg

#ifndef TASK_H
#define TASK_H

// Module inclusions
#include <inttypes.h>
#include "visual/colors.h"

// ========================= Constants and Macros ========================== //
#define TASK_TITLE_MAX_LENGTH 32    // max number of chars in a title
#define TASK_DESCRIPTION_MAX_LENGTH 512 // max number of chars in a description

// default fields: used when a task has no title or description
#define TASK_DEFAULT_TITLE "(no title)"
#define TASK_DEFAULT_DESCRIPTION "(no description)"


// ============================== Task Struct ============================== //
typedef struct _Task
{
    char* title;                    // the title of the task
    char* description;              // the description of the task
    uint64_t id;                    // unique task ID
    uint8_t is_complete;            // whether or not the task is finished
    char color[COLOR_MAX_LENGTH];   // color string
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

// Sets the task's color string. If the pointer is NULL, the default color is
// used instead.
void task_set_color(Task* task, char* name);

// Takes in a Task struct and attempts to create a dynamically-allocates string
// that repesents the task. The returned pointer must be freed after it's used.
// On failure, NULL is returned.
char* task_to_string(Task* task);


// ========================== File String Parsing ========================== //
// Takes in a pointer to a task and generates a string used by the scribe to
// write a TaskList out to a file. Returns NULL on failure.
char* task_get_scribe_string(Task* task);

// Takes in a header string (generated by 'task_get_scribe_string') and tries
// to create a new Task struct with its information. Returns NULL on failure.
Task* task_new_from_scribe_string(char* string);

#endif
