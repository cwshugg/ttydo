// This header file defines a series of functions to read and write tasks and
// task lists to and from disk.
//
//      Connor Shugg

// Module inclusions
#include "tasklist.h"

// Takes in a pointer to a TaskList and attempts to write it out to disk.
// Returns 0 on success and a non-zero value on failure.
int save_task_list(TaskList* list);

// Takes in the name of a TaskList and attempts to load it in from disk.
// On success, a dynamically-allocated TaskList pointer is returned. Otherwise,
// NULL is returned
TaskList* load_task_list(char* name);
