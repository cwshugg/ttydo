// A module that implements the 'list' command handler, along with any helper
// functions needed.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "handlers.h"
#include "../utils.h"

// Function prototypes
int handle_list_help(Command* comm, int argc, char** args);
int handle_list_new(Command* comm, int argc, char** args);


// ============================== Initializer ============================== //
Command* init_command_list()
{
    // main command
    Command* result = command_new("List", "l", "list",
        "Provides various actions to create, view, and modify task lists.",
        handle_list);
    if (!result) { return NULL; }
    
    // sub-commands
    if (command_init_subcommands(result, 2)) { return NULL; }
    result->subcommands[0] = command_new("Help", "h", "help",
        "Shows a list of support sub-commands.",
        handle_list_help);
    result->subcommands[1] = command_new("New", "n", "new",
        "Creates a new task list.",
        handle_list_new);

    // check each sub-command - if one wasn't initialized, return NULL
    for (int i = 0; i < result->subcommands_length; i++)
    {
        if (!result->subcommands[i])
        { return NULL; }
    }

    return result;
}


// ================================ Handler ================================ //
// The 'list' command handler
int handle_list(Command* comm, int argc, char** args)
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

    // try to match sub-commands
    for (int i = 0; i < comm->subcommands_length; i++)
    {
        Command* sub = comm->subcommands[i];
        if (command_match(sub, args[0]))
        { sub->handler(sub, argc - 1, args + 1); }
    }

    return 0;
}

// Handles the 'help' sub command
int handle_list_help(Command* comm, int argc, char** args)
{
    int num_commands = 2;
    int content_length = 300 * num_commands;

    // make a title and box description
    //char* title = "Supported Sub-Commands ('list')";
    char content[content_length];
    memset(content, 0, content_length);

    

    return 0;
}

// Handles the 'new' sub command.
int handle_list_new(Command* comm, int argc, char** args)
{
    if (argc < 1)
    {
        printf("Usage: \"list new <NAME>\"\n");
        return 0;
    }

    // make a new task list
    char* name = args[0];
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
