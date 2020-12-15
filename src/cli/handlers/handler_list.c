// A module that implements the 'list' command handler, along with any helper
// functions needed.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "handlers.h"
#include "../utils.h"

// Function prototypes
int handle_list_help(Command* comm, int argc, char** args);
int handle_list_new(Command* comm, int argc, char** args);
int handle_list_delete(Command* comm, int argc, char** args);


// ============================== Initializer ============================== //
Command* init_command_list()
{
    // main command
    Command* result = command_new("List", "l", "list",
        "Provides various actions to create, view, and modify task lists.",
        handle_list);
    if (!result) { return NULL; }
    
    // sub-commands
    if (command_init_subcommands(result, 3)) { return NULL; }
    result->subcommands[0] = command_new("Help", "h", "help",
        "Shows a list of support sub-commands.",
        handle_list_help);
    result->subcommands[1] = command_new("New", "n", "new",
        "Creates a new task list.",
        handle_list_new);
    result->subcommands[2] = command_new("Delete", "d", "delete",
        "Deletes an existing task list.",
        handle_list_delete);

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
            sprintf(number, "%d.", i + 1);
            printf("%-3s %s\n", number, tasklists[i]->name);
        }
        return 0;
    }

    // match the help command (takes in a different command argument)
    Command* help_comm = comm->subcommands[0];
    if (command_match(help_comm, args[0]))
    { return help_comm->handler(comm, argc - 1, args + 1); }

    // try to match sub-commands
    for (int i = 1; i < comm->subcommands_length; i++)
    {
        Command* sub = comm->subcommands[i];
        if (command_match(sub, args[0]))
        { return sub->handler(sub, argc - 1, args + 1); }
    }

    printf("Sub-Command not found. (Try 'ttydo list help')\n");

    return 0;
}

// Handles the 'help' sub command
int handle_list_help(Command* comm, int argc, char** args)
{
    // print out the sub-command array
    return print_subcommands(comm, "Supported Sub-Commands ('list')");
}

// Handles the 'new' sub command.
int handle_list_new(Command* comm, int argc, char** args)
{
    if (argc < 1)
    {
        printf("Usage: 'list new <NAME>'\n");
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

// Handles the 'delete' sub command
int handle_list_delete(Command* comm, int argc, char** args)
{
    if (argc < 1)
    {
        printf("Usage: 'list delete \"<NAME>\"' / 'list delete <NUMBER>'\n");
        return 0;
    }

    // if we don't have any task lists, return
    if (tasklist_array_length == 0)
    {
        printf("You don't have any task lists.\n");
        return 0;
    }

    // take in the first argument and attempt to convert it to an integer
    char* end;
    errno = 0;
    long index = strtol(args[0], &end, 10);

    // if the index is zero, we'll assume parsing failed, and we'll try
    // to find the index by interpreting the argument as a task list name
    int temp = 0;
    while (temp < tasklist_array_length && index == 0)
    {
        // compare at most TASK_LIST_NAME_MAX_LENGTH characters. If the
        // current list matches the name, 
        if (!strncmp(tasklists[temp]->name, args[0],
            TASK_LIST_NAME_MAX_LENGTH))
        { index = temp + 1; }
        // increment temporary index
        temp++;
    }

    // if we didn't find an index, print and continue
    if (index == 0 || index > tasklist_array_length)
    {
        eprintf("Couldn't find a task list with name/number \"%s\".\n", args[0]);
        // print an additional message for incorrect numbers
        if (index != 0)
        { printf("Numbers must be between 1 and %d.\n", tasklist_array_length); }
        return 0;
    }
    index--; // knock index back down to be an array index

    // save the task list's name for the success message
    char lname[TASK_LIST_NAME_MAX_LENGTH] = {'\0'};
    strcpy(lname, tasklists[index]->name);

    // remove from the task list. On failure, print a message and return
    if (tasklist_array_remove(index))
    {
        eprintf("Failed to delete task list.\n");
        return 1;
    }

    // print a success message and return
    printf("Task list \"%s\" deleted successfully.\n", lname);
    return 0;
}
