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
int handle_task_delete(Command* comm, int argc, char** args);
int handle_task_mark(Command* comm, int argc, char** args);
int handle_task_view(Command* comm, int argc, char** args);
int truncate_string(char* original, char* copy, int max_length);
Task* find_task(TaskList* list, char* input, char** title);
int delete_all_tasks(TaskList* list);
int mark_all_tasks(TaskList* list);
int display_task(Task* task);


// ============================== Initializer ============================== //
Command* init_command_task()
{
    // main command
    Command* result = command_new("Task", "t", "task",
        "Provides various actions to add, remove, and toggle individual tasks.",
        handle_task);
    if (!result) { return NULL; }
    
    // sub-commands
    if (command_init_subcommands(result, 5)) { return NULL; }
    result->subcommands[0] = command_new("Help", "h", "help",
        "Shows a list of supported sub-commands.",
        handle_task_help);
    result->subcommands[1] = command_new("Add", "a", "add",
        "Creates a new task within a task list.",
        handle_task_add);
    result->subcommands[2] = command_new("Delete", "d", "delete",
        "Deletes a task from a task list.",
        handle_task_delete);
    result->subcommands[3] = command_new("Mark", "m", "mark",
        "Marks a given task as complete (or incomplete).",
        handle_task_mark);
    result->subcommands[4] = command_new("View", "v", "view",
        "Displays a given task's full title and description.",
        handle_task_view);

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
            int text_max_length = TASK_LIST_NAME_MAX_LENGTH + 32;
            char text[text_max_length];
            memset(text, 0, text_max_length);
            snprintf(text, text_max_length, "%s - ", tasklists[i]->name);
            if (tasklists[i]->size > 0)
            {
                snprintf(text + strlen(text), 64, "%d/%d completed",
                         completions, tasklists[i]->size);
            }
            else
            { snprintf(text + strlen(text), 6, "empty"); }
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

    eprintf("Sub-Command not found. (Try 'ttydo task help')\n");
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

    // if too few arguments were given, print a usage message
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
    // ---------- title ---------- //
    char title[TASK_TITLE_MAX_LENGTH + 1] = {'\0'};
    int title_length = truncate_string(args[1], title,
                                       TASK_TITLE_MAX_LENGTH);
    // ------- description ------- //
    char desc[TASK_DESCRIPTION_MAX_LENGTH + 1] = {'\0'};
    int desc_length = truncate_string(args[2], desc,
                                      TASK_DESCRIPTION_MAX_LENGTH);

    // replace any newlines with spaces
    replace_string_non_printables(title, title_length);
    replace_string_non_printables(desc, desc_length);

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

// Handles the 'delete' sub command
int handle_task_delete(Command* comm, int argc, char** args)
{
    // if we don't have any lists, print and return
    if (tasklist_array_length == 0)
    {
        printf("You don't have any task lists.\n");
        return 0;
    }

    // if too few arguments were given, print a usage message
    if (argc < 2)
    {
        print_usage("task delete <LIST> <TASK>");
        printf("Where <LIST> is either a list's name or number.\n");
        printf("Where <TASK> is either a task's name or number.\n");
        // print wildcard info
        printf("Replacing <TASK> with \"%s\" will delete all tasks from the list.\n",
               WILDCARD_ALL);
        return 0;
    }

    // take the first argument and attempt to locate the correct list
    int tl_index = tasklist_array_find(args[0]);
    if (tl_index < 0)
    {
        eprintf("Couldn't find a task list with name/number \"%s\".\n", args[0]);
        fprintf(stderr, "Numbers must be between 1 and %d.\n", tasklist_array_length);
        return 1;
    }
    TaskList* list = tasklists[tl_index];

    // if the list has no entries, return
    if (list->size == 0)
    {
        printf("This list has no tasks.\n");
        return 0;
    }

    // check for the '*' wildcard
    if (!strncmp(args[1], WILDCARD_ALL, 1) && strlen(args[1]) == 1)
    { return delete_all_tasks(list); }

    // next, attempt to find the task within the task list
    char* title = NULL;
    Task* task = find_task(list, args[1], &title);

    // if we didn't find a task, print and return
    if (!task)
    {
        eprintf("Couldn't find a task within \"%s\" with name/number \"%s\".\n",
                list->name, title);
        fprintf(stderr, "Task numbers for this list must be between 1 and %d.\n",
                list->size);
        free(title);
        return 1;
    }
    free(title);

    // attempt to remove the task from the list
    if (!task_list_remove(list, task))
    { fatality(1, "Failed to remove task from the list.\n"); }
    
    // free the removed task's memory and save the task list
    task_free(task);
    if (save_task_list(list))
    { fatality(1, "Failed to write to disk."); }

    return 0;
}

// Handles the 'mark' sub-command
int handle_task_mark(Command* comm, int argc, char** args)
{
    // if we don't have any lists, print and return
    if (tasklist_array_length == 0)
    {
        printf("You don't have any task lists.\n");
        return 0;
    }

    // if too few arguments were given, print a usage message
    if (argc < 2)
    {
        print_usage("task mark <LIST> <TASK>");
        printf("Where <LIST> is either a list's name or number.\n");
        printf("Where <TASK> is either a task's name or number.\n");
        // print wildcard info
        printf("Replacing <TASK> with \"%s\" will mark all tasks as complete. "
               "If all tasks are already marked as complete, the opposite will happen.\n",
               WILDCARD_ALL);
        return 0;
    }

    // take the first argument and attempt to locate the correct list
    int tl_index = tasklist_array_find(args[0]);
    if (tl_index < 0)
    {
        eprintf("Couldn't find a task list with name/number \"%s\".\n", args[0]);
        fprintf(stderr, "Numbers must be between 1 and %d.\n", tasklist_array_length);
        return 1;
    }
    TaskList* list = tasklists[tl_index];

    // if the list has no entries, return
    if (list->size == 0)
    {
        printf("This list has no tasks.\n");
        return 0;
    }

    // check for the '*' wildcard
    if (!strncmp(args[1], WILDCARD_ALL, 1) && strlen(args[1]) == 1)
    { return mark_all_tasks(list); }
    
    // next, attempt to find the task within the task list
    char* title = NULL;
    Task* task = find_task(list, args[1], &title);

    // if we didn't find a task, print and return
    if (!task)
    {
        eprintf("Couldn't find a task within \"%s\" with name/number \"%s\".\n",
                list->name, title);
        fprintf(stderr, "Task numbers for this list must be between 1 and %d.\n",
                list->size);
        free(title);
        return 1;
    }
    free(title);

    // invert the 'is_complete' flag for the task
    task->is_complete = !task->is_complete;
    
    // save the task list
    if (save_task_list(list))
    { fatality(1, "Failed to write to disk."); }

    return 0;
}

// Handles the 'view' sub-command
int handle_task_view(Command* comm, int argc, char** args)
{
    // if we don't have any lists, print and return
    if (tasklist_array_length == 0)
    {
        printf("You don't have any task lists.\n");
        return 0;
    }

    // if too few arguments were given, print a usage message
    if (argc < 2)
    {
        print_usage("task view <LIST> <TASK>");
        printf("Where <LIST> is either a list's name or number.\n");
        printf("Where <TASK> is either a task's name or number.\n");
        // print wildcard info
        printf("Replacing <TASK> with \"%s\" will display all tasks in full.\n",
               WILDCARD_ALL);
        return 0;
    }

    // take the first argument and attempt to locate the correct list
    int tl_index = tasklist_array_find(args[0]);
    if (tl_index < 0)
    {
        eprintf("Couldn't find a task list with name/number \"%s\".\n", args[0]);
        fprintf(stderr, "Numbers must be between 1 and %d.\n", tasklist_array_length);
        return 1;
    }
    TaskList* list = tasklists[tl_index];

    // if the list has no entries, return
    if (list->size == 0)
    {
        printf("This list has no tasks.\n");
        return 0;
    }

    // check for the '*' wildcard
    if (!strncmp(args[1], WILDCARD_ALL, 1) && strlen(args[1]) == 1)
    {
        // print a header
        int length = printf("All tasks in \"%s\":\n", list->name);
        print_horizontal_line(length - 1);

        // print every task in the list
        for (int i = 0; i < list->size; i++)
        {
            Task* task = task_list_get_by_index(list, i);
            if (!task) { continue; }
            display_task(task);
        }
        return 0;
    }

    // next, attempt to find the task within the task list (print and return if
    // the task couldn't be found)
    char* title = NULL;
    Task* task = find_task(list, args[1], &title);
    if (!task)
    {
        eprintf("Couldn't find a task within \"%s\" with name/number \"%s\".\n",
                list->name, title);
        fprintf(stderr, "Task numbers for this list must be between 1 and %d.\n",
                list->size);
        free(title);
        return 1;
    }
    free(title);

    // display the found task
    return display_task(task);
}


// =========================== Helper Functions ============================ //
// Takes in a string and a pointer to memory where the string will be copied,
// AND a maximum length. 'max_length' characters are copied over, or less,
// depending on the length of the original string.
// Returns the length that was copied.
int truncate_string(char* original, char* copy, int max_length)
{
    if (!original || !copy) { return 0; }

    int length = strlen(original);
    if (length > max_length)
    { length = max_length; }
    // copy the string
    snprintf(copy, length + 1, "%s", original);
    return length;
}

// Takes in a task list, and some sort of string input, and attempts to parse
// it as either an index or a task title. If a task is found, its pointer is
// returned. If one wasn't found, NULL is returned. The third parameter is used
// to return the modified and parsed title string. It's dynamically allocated.
Task* find_task(TaskList* list, char* input, char** cleaned_input)
{
    // attempt to convert the next argument to an integer (task index) OR
    // use it as a title name for a task. Either way, we'll search the task
    // list for the appropriate task.
    char* end;
    long index = strtol(input, &end, 10);
    Task* task = NULL;
    if (index > 0)
    { task = task_list_get_by_index(list, index - 1); }
    
    // parse the title string and replace any strange characters with
    // spaces
    char* title = calloc(TASK_TITLE_MAX_LENGTH + 1, sizeof(char));
    int title_length = truncate_string(input, title,
                                    TASK_TITLE_MAX_LENGTH);
    replace_string_non_printables(title, title_length);
    // set the return parameter
    *cleaned_input = title;

    // search by title if we dont have an index
    if (index == 0)
    { task = task_list_get_by_title(list, title); }

    return task;
}

// Takes in a task list and deletes all tasks from it. Returns 0 on success
// and a nonzero value on error.
int delete_all_tasks(TaskList* list)
{
    if (!list) { return 1; }

    // iterate through the number of tasks in the list
    while (list->size > 0)
    {
        // retrieve the task
        Task* task = task_list_get_by_index(list, 0);
        if (!task) { return 1; }

        // remove it from the list and free its memory
        task = task_list_remove(list, task);
        if (task) { task_free(task); }
    }

    // save the task list to disc
    return save_task_list(list);
}

// Takes in a task list and marks all tasks as completed, as long as at least
// one task is incomplete. If every task is already marked as completed, all
// the tasks will be set to incomplete. Returns 0 on success and a nonzero
// value on error.
int mark_all_tasks(TaskList* list)
{
    if (!list) { return 1; }

    // iterate through every task in the list and count the completions. Mark
    // any incomplete tasks as completed.
    int completions = 0;
    for (int i = 0; i < list->size; i++)
    {
        Task* task = task_list_get_by_index(list, i);
        if (!task) { continue; }

        // if the task isn't complete, mark it as so. Otherwise, if it's
        // already complete, increment the counter
        if (!task->is_complete) { task->is_complete = 1; }
        else { completions++; }
    }

    // if all of the tasks were already complete, we'll mark them all as
    // incomplete
    if (completions == list->size)
    {
        for (int i = 0; i < list->size; i++)
        {
            Task* task = task_list_get_by_index(list, i);
            if (task) { task->is_complete = 0; }
        }
    }

    // write the modified list to disk
    return save_task_list(list);
}

// Helper function that displays a single task for the 'view' sub-command.
// Returns 0 on success and a non-zero value on error.
int display_task(Task* task)
{
    char* ts = task_to_string(task);
    printf("%s\n", ts);
    if (ts) { free(ts); }
    return 0;
}
