// A module that implements the 'help' command
//
//      Connor Shugg
// The 'help' command handler

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "handlers.h"
#include "../utils.h"
#include "../../visual/terminal.h"

// Function prototypes
int handle_help_subcommand_printer(Command* comm, char* title);

// ============================== Initializer ============================== //
Command* init_command_help()
{
    // allocate main command
    Command* result = command_new("Help", "h", "help",
        "Shows a help menu on how to use ttydo.",
        handle_help);
    return result;
}


// ================================ Handler ================================ //
int handle_help(Command* comm, int argc, char** args)
{
    // make sure our command array is initialized
    if (!commands)
    { fatality(1, "Commands not initialized."); }

    // print the logo
    print_logo(NULL);
    printf("A command-line task tracker.\n");

    // print the global command list
    return print_subcommands(NULL, "Supported Commands");
}
