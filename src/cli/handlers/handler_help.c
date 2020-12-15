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
        print_box_terminal_safe("Supported Commands", box_string);
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
