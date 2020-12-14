// Module that defines the main function and turns the user's input into action.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "command.h"
#include "handlers.h"
#include "../tasklist.h"

// ======================= Globals/Macros/Prototypes ======================= //
// Command globals
int NUM_COMMANDS = 2;       // number of commands in the array
Command** commands = NULL;  // global array of commands
// Task list globals
int tasklist_array_capacity = 8; // initial cap of our global tasklist array
int tasklist_array_length = 0;   // number of task lists in the array
TaskList** tasklists = NULL;     // global array of task lists
// Function prototypes
void init_commands();
int execute_command(int argc, char** args);

// ============================= Main Function ============================= //
// Main function - takes in the user's command-line arguments and attempts to
// parse them into a command. If a matching command is found, it's executed.
int main(int argc, char** argv)
{
    // initialize the command array and our global task list
    init_commands();
    tasklist_array_init();

    // if we were given no arguments, print the intro and exit
    if (argc == 1)
    {
        print_intro();
        finish();
    }

    // take the command-line arguments (minus the first one) and match them up
    // to a command. Save the return value
    int result = execute_command(argc - 1, argv + 1);
    if (result < 0)
    { fatality(1, "Command not found. (Try 'ttydo help')"); }

    // clean up and exit
    finish();
}


// =========================== Helper Functions ============================ //
// Initializes a dynamically-allocated array of dynamically-allocated Command
// structs, each representing a different command supported by the CLI. The
// array is returned on success, and NULL is returned on failure.
void init_commands()
{
    // allocate the main command array
    commands = calloc(NUM_COMMANDS, sizeof(Command*));

    // help command
    commands[0] = command_new("Help", "h", "help",
        "Shows a help menu on how to use ttydo.",
        handle_help);
    
    // list command
    commands[1] = command_new("List", "l", "list",
        "Provides various actions to create, view, and modify task lists.",
        handle_list);
}

// Searches the command list for a command with the name given by the
// command-line arguments. If one is found, it's handler is executed, and its
// return value is returned. Otherwise, -1 is returned.
int execute_command(int argc, char** args)
{
    // make sure our command array is initialized
    if (!commands)
    { fatality(1, "Commands not initialized."); }

    // extract the first command-line argument: the command name
    char* command = *args;

    // iterate through the array of commands and try to match
    Command* comm = NULL;
    int i = 0;
    while (i < NUM_COMMANDS && !comm)
    {
        // if the shorthand OR longhand string matches, we found a command
        if (!strcmp(commands[i]->shorthand, command) ||
            !strcmp(commands[i]->longhand, command))
        { comm = commands[i]; }
        i++;
    }

    // pass the remaining command-line arguments onto the command handler
    if (comm)
    { return comm->handler(argc - 1, args + 1); }    

    // if no command could be found, return -1
    return -1;
}
