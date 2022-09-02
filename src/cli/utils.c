// Implements the utility function definitions from utils.h
//
//      Connor Shugg

// Module inclusions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "utils.h"
#include "../visual/terminal.h"
#include "../scribe.h"

// ======================= Globals/Macros/Prototypes ======================= //
// Command globals
extern int NUM_COMMANDS;    // number of commands
extern Command** commands;  // dynamically-allocated command array
// Task list globals
extern int tasklist_array_capacity; // initial cap of our global tasklist array
extern int tasklist_array_length;   // number of task lists in the array
extern TaskList** tasklists;        // global array of task lists
// Function prototypes
void clean_up();


// ========================= Error/Exit Functions ========================== //
void fatality(int exit_code, char* message)
{
    fprintf(stderr, "Fatal ");

    // if we didn't get a message, print a default one
    if (!message)
    { eprintf("(no message provided)\n"); }
    else
    { eprintf("%s\n", message); }

    // clean up and exit
    clean_up();
    exit(exit_code);
}

void finish()
{
    // clean up memory and exit
    clean_up();
    exit(0);
}

void eprintf(const char* format, ...)
{
    fprintf(stderr, "Error: ");

    // if the format is null, print a default message and return
    if (!format)
    {
        fprintf(stderr, "(no message given)");
        return;
    }

    // print the variable arguments
    va_list arg;
    va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);
}

void wprintf(const char* format, ...)
{
    fprintf(stdout, "Warning: ");

    // if the format is null, print a default message and return
    if (!format)
    {
        fprintf(stdout, "(no message given)");
        return;
    }

    // print the variable arguments
    va_list arg;
    va_start(arg, format);
    vfprintf(stdout, format, arg);
    va_end(arg);
}

// Cleans all the memory allocated by the CLI. Used both when exiting normally
// and exiting with a fatal error.
void clean_up()
{
    // iterate through the commands and free each of them
    if (commands)
    {
        for (int i = 0; i < NUM_COMMANDS; i++)
        { command_free(commands[i]); }
        free(commands);
    }
    // free the tasklist array
    tasklist_array_free();
}


// ========================== Printing Functions =========================== //
void print_intro()
{
    // check our tasklist array - if we don't have any task lists, print the
    // logo and a help message
    if (tasklists && tasklist_array_length == 0)
    {
        print_logo(NULL);
        printf("A command-line task tracker. Try 'ttydo help' to see what you can do with it.\n");
        return;
    }

    // otherwise, we'll print out the lists currently stored
    int print_amount = tasklist_array_length;
    if (tasklist_array_length == 1)
    { printf("You have 1 task list.\n"); }
    else
    { printf("You have %d task lists.\n", tasklist_array_length); }

    // count those that actually have tasks
    int filled_amount = 0;
    for (int i = 0; i < tasklist_array_length; i++)
    { filled_amount += tasklists[i]->size > 0; }
    
    // print a message about only SOME being full
    if (tasklist_array_length > filled_amount)
    {
        if (filled_amount == 0)
        {
            if (tasklist_array_length > 1)
            { printf("None of them contain any tasks.\n"); }
            else
            { printf("It doesn't contain any tasks.\n"); }
        }
        else if (filled_amount == 1)
        { printf("Only 1 contains tasks.\n"); }
        else
        { printf("Only %d contain tasks.\n", filled_amount); }
    }

    // iterate and print each in box form
    for (int i = 0; i < print_amount; i++)
    {
        // only print if the list has tasks
        if (tasklists[i]->size > 0)
        {
            BoxStack* bs = task_list_to_box_stack(tasklists[i], 1);
            if (!bs)
            { eprintf("Couldn't print task list: %s.\n", tasklists[i]->name); }

            box_stack_print(bs);
            box_stack_free(bs);
        }
    }
}

void print_logo(char* prefix)
{
    // if we weren't given a prefix, use a blank string
    if (!prefix)
    { prefix = ""; }
    
    // ========================== Row 1 ========================== //
    //            T1      T2               D
    printf("%s  \u2588  \u2588        \u2588", prefix);
    printf("\n");
    
    // ========================== Row 2 ========================== //
    //        ---------- T1 ------------- -------- T2 --------
    printf("%s\u2580\u2580\u2588\u2580\u2580\u2588\u2580\u2580", prefix);
    //      ---- Y ----- ------ D ------- -------- O ---------
    printf("\u2588  \u2588\u2580\u2580\u2588\u2580\u2580\u2588");
    printf("\n");
    
    // ========================== Row 3 ========================== //
    //            T1      T2    ---------- Y -----------
    printf("%s  \u2588  \u2588  \u2588\u2584\u2584\u2588", prefix);
    //      ------- D ------- ------- O --------
    printf("\u2584\u2584\u2588\u2584\u2584\u2588");
    printf("\n");
    
    // ========================== Row 4 ========================== //
    //          --------- underline ---------- ----- Y ------
    printf("%s  \u2584\u2584\u2584\u2584\u2584 \u2584  \u2588", prefix);
    //       --------- underline ----------
    printf(" \u2584\u2584\u2584\u2584\u2584");
    printf("\n");

    // ========================== Row 5 ========================== //
    //                ---------- Y -----------
    printf("%s        \u2580\u2580\u2580\u2580", prefix);
    printf("\n");
}

void print_box_terminal_safe(char* title, char* text)
{
    int terminal_width = get_terminal_width();

    // print title
    int top_line_chars_remaining = terminal_width;
    if (title)
    {
        int title_length = strlen(title) + 2;
        if (title_length < terminal_width)
        { top_line_chars_remaining -= title_length + 2; }

        printf("%s%s %s ", BOX_H_LINE, BOX_H_LINE, title);
    }
    // print remainder of top line
    for (int i = 0; i < top_line_chars_remaining; i++)
    { printf("%s", BOX_H_LINE); }

    // print box text
    if (text)
    { printf("\n%s\n", text); }

    // print bottom line and newline
    for (int i = 0; i < terminal_width; i++)
    { printf("%s", BOX_H_LINE); }
    printf("\n");
}

void print_horizontal_line(int length)
{
    for (int i = 0; i < length; i++)
    { printf(H_LINE); }
    printf("\n");
}

int print_subcommands(Command* comm, char* title)
{
    // if we were given a null title, return
    if (!title) { return 1; }

    // by default, use global list
    Command** arr = commands;
    int arr_length = NUM_COMMANDS;
    // if we were given a non-null command, use its sub-commands instead
    if (comm)
    {
        arr = comm->subcommands;
        arr_length = comm->subcommands_length;
    }

    // we'll make a generous guess that each command string will be under 256
    // characters. Since each will also have a '\n'.... we'll just round up to
    // 300 to be safe.
    char* box_string = calloc(300 * arr_length, sizeof(char));
    if (!box_string) { return 1; }
    
    // maintain the box string's length and use a maximum for each comm string
    int box_string_length = 0;
    int comm_string_max_length = 256;

    // iterate through each command build a giant string to go in a box
    int longest_length = 0;
    for (int i = 0; i < arr_length; i++)
    {
        // make the string, append it to the master string, then free it
        char* comm_string = command_to_string(arr[i]);
        box_string_length += snprintf(box_string + box_string_length,
                                      comm_string_max_length, "%s",
                                      comm_string);

        // append a newline, if necessary
        if (i < arr_length - 1)
        {
            box_string_length += snprintf(box_string + box_string_length,
                                          2, "\n");
        }

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
        print_box_terminal_safe(title, box_string);
        // free the box string and return
        free(box_string);
        return 0;
    }
    
    // create a box and free the master string (since the box will copy it)
    Box* box = box_new(0, 0, title, box_string);
    free(box_string);
    if (!box)
    { return 1; }

    // adjust the box to fit the text, print, and free it
    box_adjust_to_text(box, 1);
    box_print(box);
    box_free(box);
    return 0;
}

void print_list_item(int index, char* string)
{
    if (!string) { return; }

    int number_max_length = 8;
    char number[number_max_length];
    memset(number, 0, number_max_length);
    snprintf(number, number_max_length, "%d.", index);
    printf("%-3s %s\n", number, string);
}

void print_usage(char* usage)
{
    printf("Usage: '%s'\n", usage);
}

// ======================= Task List Array Functions ======================= //
int tasklist_array_init()
{
    // first, count the number of task lists stored on disk in the ttydo
    // directory. If there are more than our initial capacity, we'll want to
    // allocate a larger array.
    char** list_names = NULL;
    int list_count = count_saved_task_lists(&list_names);

    // sort the array of list names
    sort_string_array((const char**) list_names, list_count);

    // calculate the nearest power of two relative to the list count
    int list_cap = 0;
    int i = 31;
    while (i >= 0 && !list_cap)
    {
        // shift the ith bit - if it's 1, we found the MSB that's = 1.
        // That's our log_base2 of list_count
        if (list_count >> i)
        { list_cap = i + 1; }
        i--;
    }
    // use the log_base2 of the list count to calculate a power-of-two capacity
    // for our list array (with a minimum value of 8)
    if (list_cap > 3) { list_cap = 1 << list_cap; }
    else { list_cap = 8; }
    tasklist_array_capacity = list_cap;

    // use the capacity to create an appropriately-size array
    tasklists = calloc(tasklist_array_capacity, sizeof(TaskList*));
    if (!tasklists)
    { return 1; }

    // iterate through the task list files and load them into memory
    for (int i = 0; i < list_count; i++)
    {
        // load the file, increase the array length, and free the path string
        tasklists[i] = load_task_list(list_names[i]);
        tasklist_array_length++;
        free(list_names[i]);
    }
    free(list_names);

    return 0;
}

void tasklist_array_free()
{
    if (!tasklists) { fatality(1, "Task list array not initialized."); }

    if (tasklist_array_length > 0)
    {
        // iterate up to tasklist_array_length times and free each tasklist
        for (int i = 0; i < tasklist_array_length; i++)
        { task_list_free(tasklists[i]); }
    }

    // free the tasklist pointer itself and set it back to NULL
    free(tasklists);
    tasklists = NULL;
}

int tasklist_array_add(TaskList* list)
{
    // check our global list, and for null input
    if (!tasklists) { fatality(1, "Task list array not initialized."); }
    if (!list) { return 1; }

    // make sure we have enough room in the array. We'll double the capacity
    // if necessary
    if (tasklist_array_length == tasklist_array_capacity)
    {
        tasklist_array_capacity <<= 1; // multiply by 2
        tasklists = realloc(tasklists, tasklist_array_capacity * sizeof(TaskList*));
        if (!tasklists)
        { fatality(1, "Task list array couldn't be expanded."); }
    }

    // add the task list to the next available index, save it to a file, and
    // return 0. (Exit on failed save attempt)
    tasklists[tasklist_array_length++] = list;
    if (save_task_list(list))
    { fatality(1, "Failed to write to task list to disk."); }
    return 0;
}

int tasklist_array_remove(int index)
{
    // check our global list or invalid input
    if (!tasklists) { fatality(1, "Task list array not initialized."); }
    if (index < 0 || index >= tasklist_array_length)
    { return 1; }

    // delete the task list's file on disk
    int del_result = delete_task_list(tasklists[index]);
    if (del_result)
    {
        eprintf("Failed to delete task list from disk.\n");
        return 1;
    }
    
    // free and null-out the pointer
    task_list_free(tasklists[index]);
    tasklists[index] = NULL;

    // if the are task lists that occurr after the one we just removed, we
    // need to shift them down
    if (index < tasklist_array_length - 1)
    {
        for (int i = index; i < tasklist_array_length; i++)
        { tasklists[i] = tasklists[i + 1]; }
    }

    // decrease the array size
    tasklist_array_length--;

    return 0;
}

int tasklist_array_find(char* input)
{
    // check for null input
    if (!input) { return -1; }

    // take in the first argument and attempt to convert it to an integer
    char* end;
    long index = strtol(input, &end, 10);

    // if the index is zero, we'll assume parsing failed, and we'll try
    // to find the index by interpreting the argument as a task list name
    int temp = 0;
    while (temp < tasklist_array_length && index == 0)
    {
        // compare at most TASK_LIST_NAME_MAX_LENGTH characters. If the
        // current list matches the name, 
        if (!strncmp(tasklists[temp]->name, input,
            TASK_LIST_NAME_MAX_LENGTH))
        { index = temp + 1; }
        // increment temporary index
        temp++;
    }

    // if we didn't find an index, print and continue
    if (index == 0 || index > tasklist_array_length)
    { return -1; }
    
    // return the array index
    return index - 1;
}

// ======================== Other Helper Functions ========================= //
void sort_string_array(const char** strings, int length)
{
    // check for null entry
    if (!strings) { return; }

    // call qsort
    qsort(strings, length, sizeof(const char*), sort_string_array_cmp);
}

int sort_string_array_cmp(const void* a, const void* b)
{ return strcmp(*(const char**) a, *(const char**) b); }

void replace_string_non_printables(char* string, int max_length)
{
    if (!string) { return; }

    // iterate through each character and make replacements
    int i = 0;
    char* current = string;
    while (i < max_length && current)
    {
        if (*current == '\n' || *current == '\t' || *current == '\r' ||
            *current == '\v' || (int) *current < 32)
        { *current = ' '; }

        i++;
        current++;
    }
}
