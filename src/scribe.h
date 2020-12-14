// This header file defines a series of functions to read and write tasks and
// task lists to and from disk.
//
//      Connor Shugg

#ifndef SCRIBE_H
#define SCRIBE_H

// Module inclusions
#include "tasklist.h"

// Takes in a pointer to a TaskList and attempts to write it out to disk.
// Returns 0 on success and a non-zero value on failure.
int save_task_list(TaskList* list);

// Takes in the name of a TaskList and attempts to load it in from disk.
// On success, a dynamically-allocated TaskList pointer is returned. Otherwise,
// NULL is returned
TaskList* load_task_list(char* name);

// Attempts to count the number of saved task lists in the ttydo directory.
// The number counted is returned, and the names of the lists are saved in a
// dynamically-allocated char** pointer (whose address is provided by the
// caller).
int count_saved_task_lists(char*** list_names);

#endif
