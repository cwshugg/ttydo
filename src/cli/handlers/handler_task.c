// Implements the 'task' command handler and initializer.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include "handlers.h"
#include "../utils.h"

// Function prototypes
int handle_task_help(Command* comm, int argc, char** args);


// ============================== Initializer ============================== //
Command* init_command_task()
{
    // main command
    Command* result = command_new("Task", "t", "task",
        "Provides various actions to add, remove, and toggle individual tasks.",
        handle_task);
    if (!result) { return NULL; }
    
    // sub-commands
    if (command_init_subcommands(result, 1)) { return NULL; }
    result->subcommands[0] = command_new("Help", "h", "help",
        "Shows a list of support sub-commands.",
        handle_task_help);

    // check each sub-command - if one wasn't initialized, return NULL
    for (int i = 0; i < result->subcommands_length; i++)
    {
        if (!result->subcommands[i])
        { return NULL; }
    }

    return result;
}


// ================================ Handler ================================ //
int handle_task(Command* comm, int argc, char** args)
{
    return 0;
}

// Handles the 'help' sub command
int handle_task_help(Command* comm, int argc, char** args)
{
    // print out the sub-command array
    return print_subcommands(comm, "Supported Sub-Commands ('list')");
}
