// Module that defines the main function and turns the user's input into action.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "command.h"
#include "../visual/box.h"
#include "../visual/terminal.h"

// ======================= Globals/Macros/Prototypes ======================= //
// Globals
int NUM_COMMANDS = 2;
Command** commands = NULL;
// Function prototypes
void init_commands();
int handle_help(char** args);
int handle_list(char** args);

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

    // TEST PRINT HELP
    handle_help(NULL);

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


// =========================== Command Handlers ============================ //
// Handler for the 'help' command.
int handle_help(char** args)
{
    // make sure our command array is initialized
    if (!commands)
    { fatality(1, "Commands not initialized."); }

    // we'll make a generous guess that each command string will be under 256
    // characters. Since each will also have a '\n'.... we'll just round up to
    // 300 to be safe.
    char* box_string = calloc(300 * NUM_COMMANDS, sizeof(char));
    if (!box_string) { return 1; }

    // iterate through each command build a giant string to go in a box
    int longest_length = 0;
    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        // make the string, append it to the master string, then free it
        char* comm_string = command_to_string(commands[i]);
        strcat(box_string, comm_string);

        // append a newline, if necessary
        if (i < NUM_COMMANDS - 1)
        { strcat(box_string, "\n"); }

        // update the maximum line length
        int length = strlen(comm_string) + 1;
        if (length > longest_length)
        { longest_length = length; }

        // free the command string
        free(comm_string);
    }
    
    // check the terminal width - if it's not long enough to print a box,
    // we'll instead print out the commands without a box (so the text will
    // wrap around)
    int terminal_width = get_terminal_width();
    if (longest_length + 4 > terminal_width)
    {
        printf("Support commands:\n");
        for (int i = 0; i < terminal_width; i++) { printf("-"); }
        printf("\n%s\n", box_string);
        for (int i = 0; i < terminal_width; i++) { printf("-"); }
        printf("\n");

        // free the box string and return
        free(box_string);
        return 0;
    }
    
    // create a box and free the master string (since the box will copy it)
    Box* box = box_new(0, 0, "Supported Commands", box_string);
    free(box_string);
    if (!box)
    { return 1; }

    // adjust the box to fit the text, print, and free it
    box_adjust_to_text(box, 1);
    box_print(box);
    box_free(box);
    return 0;
}

// The 'list' command handler
int handle_list(char** args)
{
    return 0;
}
