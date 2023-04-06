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
#include "../../scribe.h"

// Function prototypes
int handle_list_help(Command* comm, int argc, char** args);
int handle_list_add(Command* comm, int argc, char** args);
int handle_list_delete(Command* comm, int argc, char** args);
int handle_list_rename(Command* comm, int argc, char** args);
int handle_list_color(Command* comm, int argc, char** args);
int handle_list_view(Command* comm, int argc, char** args);
int list_name_is_valid(char* name);


// ============================== Initializer ============================== //
Command* init_command_list()
{
    // main command
    Command* result = command_new("List", "l", "list",
        "Provides various actions to create, view, and modify task lists.",
        handle_list);
    if (!result) { return NULL; }
    
    // sub-commands
    if (command_init_subcommands(result, 6)) { return NULL; }
    result->subcommands[0] = command_new("Help", "h", "help",
        "Shows a list of supported sub-commands.",
        handle_list_help);
    result->subcommands[1] = command_new("Add", "a", "add",
        "Creates a new task list.",
        handle_list_add);
    result->subcommands[2] = command_new("Delete", "d", "delete",
        "Deletes an existing task list.",
        handle_list_delete);
    result->subcommands[3] = command_new("Rename", "r", "rename",
        "Renames an existing task list.",
        handle_list_rename);
    result->subcommands[4] = command_new("Color", "c", "color",
        "Sets an existing task list's color.",
        handle_list_color);
    result->subcommands[5] = command_new("View/Verbose", "v", "view",
        "Displays all tasks in the list, their numbers, and their full descriptions.",
        handle_list_view);

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
        int count_message_max_length = 128;
        char count_message[count_message_max_length];
        memset(count_message, 0, count_message_max_length);
        int length = snprintf(count_message,
                              count_message_max_length,
                              "You have %d task list",
                              tasklist_array_length);
        // add "s", if necessary, and a period at the end
        if (tasklist_array_length > 1)
        { length += snprintf(count_message + length, 3, "s."); }
        else
        { length += snprintf(count_message + length, 2, "."); }

        // print the message, and a divider line
        printf("%s\n", count_message);
        print_horizontal_line(length);

        // iterate through all lists
        for (int i = 0; i < tasklist_array_length; i++)
        {
            int text_max_length = TASK_LIST_NAME_MAX_LENGTH + 8;
            char text[text_max_length];
            memset(text, 0, text_max_length);
            snprintf(text, text_max_length, "%s", tasklists[i]->name);
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
    {
        eprintf("Sub-Command not found. (Try 'ttydo list help')\n");
        return 1;
    }

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
int handle_list_add(Command* comm, int argc, char** args)
{
    if (argc < 1)
    {
        print_usage("list add <NAME>");
        return 0;
    }

    // if the given name doesn't start with a letter, complain and return
    if (!list_name_is_valid(args[0]))
    {
        eprintf("List names must begin with a letter.\n");
        return 1;
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

    //printf("Successfully created a new list ('%s').\n", name);
    return 0;
}

// Handles the 'delete' sub command
int handle_list_delete(Command* comm, int argc, char** args)
{
    if (argc < 1)
    {
        print_usage("list delete <LIST>");
        printf("Where <LIST> is either a list's name or number.\n");
        printf("Replacing <LIST> with \"%s\" will delete all lists.\n",
               WILDCARD_ALL);
        return 0;
    }

    // if we don't have any task lists, return
    if (tasklist_array_length == 0)
    {
        printf("You don't have any task lists.\n");
        return 0;
    }

    // check for the 'ALL' wildcard
    if (!strncmp(args[0], WILDCARD_ALL, 1) && strlen(args[0]) == 1)
    {
        // iterate through all tasklists and remove them (we'll do this in
        // reverse order, so we dont' have to deal with the indexes of entries
        // changing when tasklist_array_remove() shifts them up)
        int result = 0;
        int length = tasklist_array_length;
        for (int i = length - 1; i >= 0; i--)
        { result = result || tasklist_array_remove(i); }

        // if some failure occurred, print and return a non-zero
        if (result)
        {
            eprintf("Failed to delete all task lists.\n");
            return 1;
        }
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

    // remove from the task list. On failure, print a message and return
    if (tasklist_array_remove(index))
    {
        eprintf("Failed to delete task list.\n");
        return 1;
    }

    return 0;
}

// Handles the 'rename' sub command
int handle_list_rename(Command* comm, int argc, char** args)
{
    if (argc < 2)
    {
        print_usage("list rename <LIST> <NEW NAME>");
        printf("Where <LIST> is either a list's name or number.\n");
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
    TaskList* list = tasklists[index];

    // make a local copy of the proposed new name, with length restrictions
    char new_name[TASK_LIST_NAME_MAX_LENGTH + 1] = {'\0'};
    snprintf(new_name, TASK_LIST_NAME_MAX_LENGTH + 1, "%s", args[1]);

    // check the name for validity
    if (!list_name_is_valid(new_name))
    {
        eprintf("List names must begin with a letter.\n");
        return 1;
    }

    // delete the old list file from memory
    if (delete_task_list(list))
    { eprintf("Couldn't delete old list file.\n"); }

    // duplicate the new name and save the list to a new file
    free(list->name);
    list->name = strdup(new_name);
    return save_task_list(list);
}

int handle_list_color(Command* comm, int argc, char** args)
{
    if (argc < 2)
    {
        print_usage("list color <LIST> <COLOR>");
        printf("Where <LIST> is either a list's name or number.\n");
        printf("Where <COLOR> is the name of the desired color.\n");
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
    TaskList* list = tasklists[index];

    // get the length of the given color value
    char* value = args[1];
    int value_length = strlen(value);

    // check for a match before setting
    const char* cstr = color_from_name(value);
    if (!cstr)
    {
        eprintf("Couldn't find a color named \"%s\".\n", value);
        fprintf(stderr, "The color options are:\n");
        int ccount = color_count();
        for (int i = 0; i < ccount; i++)
        {
            fprintf(stderr, " - %s%s\n" C_NONE,
                    color_from_index(i),
                    color_name_from_index(i));
        }
        return 1;
    }

    // set the color and save
    task_list_set_color(list, value);
    return save_task_list(list);
}


int handle_list_view(Command* comm, int argc, char** args)
{
    // check for the correct command-line arguments
    if (argc < 1)
    {
        print_usage("list view <LIST>");
        printf("Where <LIST> is either a list's name or number.\n");
        return 0;
    }

    // if we don't have any task lists, there's no point
    if (tasklist_array_length == 0)
    {
        printf("You don't have any task lists.\n");
        return 0;
    }

    // attempt to find the index of the matching task list
    int index = tasklist_array_find(args[0]);
    if (index < 0)
    {
        eprintf("Couldn't find a task list with name/number \"%s\".\n", args[0]);
        fprintf(stderr, "Numbers must be between 1 and %d.\n", tasklist_array_length);
        return 0;
    }
    TaskList* list = tasklists[index];
    
    // print the list's title, then iterate across the list's linked elements to
    // retrieve each task
    printf("%s\n", list->name);
    int i = 0;
    TaskListElem* current = list->head;
    while (i++ < list->size && current)
    {
        char* tstr = task_to_string(current->task);
        printf("%d. %s\n", i, tstr);
        free(tstr);

        // move to the next task
        current = current->next;
    }
    return 0;
}


// =========================== Helper Functions ============================ //
// Checks a given string to see if it's a valid list name. Returns 1 if so and
// 0 if not.
int list_name_is_valid(char* name)
{
    if (!name) { return 0; }

    // if the name doesn't begin with a letter, return 1
    if (!name || *name < 65 || *name > 122 ||
        (*name > 90 && *name < 97))
    { return 0; }
    
    return 1; // success
}
