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
int handle_list_delete(Command* comm, int argc, char** args);
int find_task_list(char* input);


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
            printf("You don't have any task lists.\n");
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

    // if we couldn't find a sub-command, try to match it as a name or
    // number of a list. If it matches one, we'll print it out
    int index = tasklist_array_find(args[0]);
    if (index >= 0)
    {
        // if the task list has tasks, print it as a box stack
        if (tasklists[index]->size > 0)
        {
            BoxStack* bs = task_list_to_box_stack(tasklists[index], 1);
            if (!bs) { eprintf("Couldn't print task list."); }
            // print the box stack
            box_stack_print(bs);
            box_stack_free(bs);
        }
        else
        { printf("The list '%s' has no tasks.\n", tasklists[index]->name); }
    }
    else
    { eprintf("Sub-Command not found. (Try 'ttydo list help')\n"); }

    return 0;
}

// Handles the 'help' sub command
int handle_list_help(Command* comm, int argc, char** args)
{
    // print out the sub-command array
    int result = print_subcommands(comm, "Supported Sub-Commands ('list')");

    // print out other message(s)
    printf("Run without arguments to view your lists.\n");
    printf("Run with a list name or number to view a list.\n");
    return result;
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

    // take the argument and try to find an index of a task list
    int index = tasklist_array_find(args[0]);
    if (index < 0)
    {
        eprintf("Couldn't find a task list with name/number \"%s\".\n", args[0]);
        fprintf(stderr, "Numbers must be between 1 and %d.\n", tasklist_array_length);
        return 0;
    }

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
