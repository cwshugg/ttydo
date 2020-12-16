// Implements the 'task' command handler and initializer.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "handlers.h"
#include "../utils.h"
#include "../../scribe.h"

// Function prototypes
int handle_task_help(Command* comm, int argc, char** args);
int handle_task_add(Command* comm, int argc, char** args);


// ============================== Initializer ============================== //
Command* init_command_task()
{
    // main command
    Command* result = command_new("Task", "t", "task",
        "Provides various actions to add, remove, and toggle individual tasks.",
        handle_task);
    if (!result) { return NULL; }
    
    // sub-commands
    if (command_init_subcommands(result, 2)) { return NULL; }
    result->subcommands[0] = command_new("Help", "h", "help",
        "Shows a list of supported sub-commands.",
        handle_task_help);
    result->subcommands[1] = command_new("Add", "a", "add",
        "Creates a new task within a task list.",
        handle_task_add);

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
    // make sure our tasklist array has been initialized
    if (!tasklists)
    { fatality(1, "Task list array has not been initialized."); }

    // if we weren't given any arguments, we'll print out a summary
    if (argc == 0)
    {
        // if we have no lists, print a message
        if (tasklist_array_length == 0)
        {
            printf("You don't have any task lists.\n");
            return 0;
        }

        char* message = "A summary of your tasks:";
        printf("%s\n", message);
        print_horizontal_line(strlen(message));
        
        // otherwise, iterate through each task list and print out statistics
        for (int i = 0; i < tasklist_array_length; i++)
        {
            // count the number of completed tasks for this list
            int completions = 0;
            TaskListElem* current = tasklists[i]->head;
            int j = 0;
            while (j < tasklists[i]->size && current)
            {
                completions += current->task->is_complete;
                current = current->next;
            }

            // print out information about the task list
            char text[TASK_LIST_NAME_MAX_LENGTH + 32] = {'\0'};
            sprintf(text, "%s - ", tasklists[i]->name);
            if (tasklists[i]->size > 0)
            {
                sprintf(text + strlen(text), "%d/%d completed", completions,
                        tasklists[i]->size);
            }
            else
            { sprintf(text + strlen(text), "empty"); }
            print_list_item(i + 1, text);
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

    eprintf("Sub-Command not found. (Try 'ttydo list help')\n");
    return 1;

    return 0;
}

// Handles the 'help' sub command
int handle_task_help(Command* comm, int argc, char** args)
{
    // print out the sub-command array
    int result = print_subcommands(comm, "Supported Sub-Commands ('task')");

    // print extra message(s)
    printf("Run without arguments to view a summary.\n");
    return result;
}

// Handles the 'add' sub command
int handle_task_add(Command* comm, int argc, char** args)
{
    // if we don't have any lists, print and return
    if (tasklist_array_length == 0)
    {
        printf("You don't have any task lists.\n");
        return 0;
    }

    // if no arguments were given, print a usage message
    if (argc < 3)
    {
        print_usage("task add <LIST> \"<TASK_NAME>\" \"<TASK_DESCRIPTION>\"");
        printf("Where <LIST> is either a list's name or number.\n");
        return 0;
    }

    // take the first argument and attempt to locate the correct list
    int index = tasklist_array_find(args[0]);
    if (index < 0)
    {
        eprintf("Couldn't find a task list with name/number \"%s\".\n", args[0]);
        fprintf(stderr, "Numbers must be between 1 and %d.\n", tasklist_array_length);
        return 1;
    }
    
    // interpret the two remaining arguments as the name and description of a
    // new task. Truncate the strings if necessary.
    char title[TASK_TITLE_MAX_LENGTH + 1] = {'\0'};
    strncpy(title, args[1], TASK_TITLE_MAX_LENGTH);
    char desc[TASK_DESCRIPTION_MAX_LENGTH + 1] = {'\0'};
    strncpy(desc, args[2], TASK_DESCRIPTION_MAX_LENGTH);

    // warn about truncations, if needed
    if (strlen(args[1]) > TASK_TITLE_MAX_LENGTH)
    { wprintf("The title is too long. It will be truncated.\n"); }
    if (strlen(args[2]) > TASK_DESCRIPTION_MAX_LENGTH)
    { wprintf("The description is too long. It will be truncated.\n"); }

    // replace any newlines with spaces
    replace_string_non_printables(title, TASK_TITLE_MAX_LENGTH);
    replace_string_non_printables(desc, TASK_DESCRIPTION_MAX_LENGTH);

    // create the new task
    Task* task = task_new(title, desc);
    if (!task) { fatality(1, "Failed to allocate memory for a new task."); }

    // add it to the correct list
    if (task_list_append(tasklists[index], task))
    { fatality(1, "Failed to add the task to the list."); }

    // save the task list
    if (save_task_list(tasklists[index]))
    { fatality(1, "Failed to write the new task to disk."); }

    return 0;
}
