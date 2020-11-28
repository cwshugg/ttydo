// This module implements scribe.h's definitions.
//
//      Connor Shugg

// Module inclusions
#include "scribe.h"

// Takes in a pointer to a TaskList and attempts to write it out to disk.
// Returns 0 on success and a non-zero value on failure.
int save_task_list(TaskList* list)
{
    // TODO
    return 1;
}

// Takes in the name of a TaskList and attempts to load it in from disk.
// On success, a dynamically-allocated TaskList pointer is returned. Otherwise,
// NULL is returned
TaskList* load_task_list(char* name)
{
    // TODO
    return NULL;
}
