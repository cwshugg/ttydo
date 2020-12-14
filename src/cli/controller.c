// Module that defines the main function and turns the user's input into action.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "command.h"

// ======================= Globals/Macros/Prototypes ======================= //
// Globals
int NUM_COMMANDS = 1;
Command** commands = NULL;
// Function prototypes
void init_commands();
int handle_help(char** args);

// ============================= Main Function ============================= //
// Main function - takes in the user's command-line arguments and attempts to
// parse them into a command. If a matching command is found, it's executed.
int main(int argc, char** argv)
{
    // if we were given no arguments, print the intro and exit
    if (argc == 1)
    {
        print_intro();
        finish();
    }

    // initialize the command array
    init_commands();

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
        "Shows a help menu on how to use ttydo.\nUsage: \"ttydo h/help\"",
        handle_help);
}


// =========================== Command Handlers ============================ //
// Handler for the 'help' command.
int handle_help(char** args)
{
    printf("Help!\n");
    return 0;
}
