// Implements the handler functions defined in handlers.h.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "handlers.h"
#include "command.h"
#include "utils.h"
#include "../visual/box.h"
#include "../visual/terminal.h"
#include "../tasklist.h"

// ======================= Globals/Macros/Prototypes ======================= //
// Command globals
extern int NUM_COMMANDS;    // reference to command count
extern Command** commands;  // reference to command array
// Task list globals
extern int tasklist_array_capacity;
extern int tasklist_array_length;
extern TaskList** tasklists;


// ======================== Header Implementations ========================= //
// The 'help' command handler
int handle_help(int argc, char** args)
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

// The 'list' command handler
int handle_list(int argc, char** args)
{
    // make sure our tasklist array has been initialized
    if (!tasklists)
    { fatality(1, "Task list array has not been initialized."); }

    // if we weren't given any arguments, print and return
    if (argc == 0)
    {
        // if we have no lists, print a message
        if (tasklist_array_length == 0)
        {
            printf("Looks like you don't have any task lists.\n");
            printf("Try 'ttydo list new' to make one.\n");
            return 0;
        }

        // make a "you have X lists" message
        char count_message[128] = {'\0'};
        sprintf(count_message, "You have %d task list", tasklist_array_length);
        if (tasklist_array_length > 1) { strcat(count_message, "s"); }
        strcat(count_message, ".");

        // print the message, and a divider line
        printf("%s\n", count_message);
        print_horizontal_line(strlen(count_message));

        // iterate through all lists
        for (int i = 0; i < tasklist_array_length; i++)
        {
            char number[8];
            sprintf(number, "%d.", i);
            printf("%-3s %s\n", number, tasklists[i]->name);
        }
        return 0;
    }

    // check for the 'new'/'n' command
    if (!strcmp(args[0], "n") || !strcmp(args[0], "new"))
    {
        if (argc < 2)
        {
            printf("Usage: \"list new <NAME>\"\n");
            return 0;
        }

        // make a new task list
        char* name = args[1];
        TaskList* list = task_list_new(name);
        if (!list)
        { fatality(1, "Failed to allocate memory for a new task list."); }

        // save add the new task list
        int add_result = tasklist_array_add(list);
        if (add_result)
        { fatality(1, "Failed to add new task list to the global array."); }

        printf("Successfully created a new list ('%s').\n", name);
        return 0;
    }

    return 0;
}