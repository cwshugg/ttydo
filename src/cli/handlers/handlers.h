// A module that defines handler functions for ttydo's commands.
//
//      Connor Shugg

#ifndef HANDLER_H
#define HANDLER_H

// Module inclusions
#include "../command.h"
#include "../../tasklist.h"

// ============================ Globals/Macros ============================= //
// Command globals
extern int NUM_COMMANDS;    // reference to command count
extern Command** commands;  // reference to command array
// Task list globals
extern int tasklist_array_capacity; // global task list array capacity
extern int tasklist_array_length;   // global task list array length
extern TaskList** tasklists;        // global task list array


// =========================== Handler Functions =========================== //
// Each of these handler functions (and each init function) is defined in each
// 'handler_*.c' source file. Each handler function returns 0 on success and a
// non-zero value on error. Each initializer function returns a dynamically
// allocated Command object on success, and NULL on failure.

// The 'help' command handler
extern int handle_help(Command* comm, int argc, char** args);
// The 'help' command initializer
extern Command* init_command_help();

// The 'list' command handler
extern int handle_list(Command* comm, int argc, char** args);
// The 'list' command initializer
extern Command* init_command_list();

#endif
