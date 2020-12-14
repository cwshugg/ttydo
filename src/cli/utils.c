// Implements the utility function definitions from utils.h
//
//      Connor Shugg

// Module inclusions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"
#include "command.h"
#include "../visual/terminal.h"
#include "../tasklist.h"
#include "../scribe.h"

// ======================= Globals/Macros/Prototypes ======================= //
// Command globals
extern int NUM_COMMANDS;    // number of commands
extern Command** commands;  // dynamically-allocated command array
// Task list globals
extern int TASKLIST_ARRAY_CAPACITY; // initial cap of our global tasklist array
extern int TASKLIST_ARRAY_LENGTH;   // number of task lists in the array
extern TaskList** tasklists;        // global array of task lists
// Function prototypes
void clean_up();


// ========================= Error/Exit Functions ========================== //
void fatality(int exit_code, char* message)
{
    // if we didn't get a message, print a default one
    if (!message)
    { fprintf(stderr, "FATAL: (no message provided)\n"); }
    else
    { fprintf(stderr, "FATAL: %s\n", message); }

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
    char* logo_prefix = "";
    printf("%s=============================\n", logo_prefix);
    print_logo(logo_prefix);
    printf("%s=============================\n", logo_prefix);
    printf("A command-line task tracker. Try 'ttydo help' to see what you can do.\n");
}

void print_logo(char* prefix)
{
    // if we weren't given a prefix, use a blank string
    if (!prefix)
    { prefix = ""; }

    // ========================== Row 1 ========================== //
    //       --------- top of T1 ---------- --------- top of T2 ----------
    printf("%s\u2580\u2580\u2588\u2580\u2580 \u2580\u2580\u2588\u2580\u2580", prefix);
    //       -- top of Y --  ---------- top of D ----------
    printf(" \u2588   \u2588 \u2588\u2580\u2580\u2580\u2584");
    //       ---------- top of O ----------
    printf(" \u2584\u2580\u2580\u2580\u2584\n");
    
    // ========================== Row 2 ========================== //
    //         T1 stem    T2 stem   ---- Y middle ----
    printf("%s  \u2588     \u2588    \u2580\u2584\u2580", prefix);
    //        D side   D side O side   O side
    printf("  \u2588   \u2588 \u2588   \u2588\n");

    // ========================== Row 3 ========================== //
    //         T1 stem    T2 stem    Y stem
    printf("%s  \u2588     \u2588     \u2588", prefix);
    //         ---------- D bottom ----------
    printf("   \u2588\u2584\u2584\u2584\u2580");
    //       ---------- O bottom ----------
    printf(" \u2580\u2584\u2584\u2584\u2580\n");
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

// ======================= Task List Array Functions ======================= //
int tasklist_array_init()
{
    // first, count the number of task lists stored on disk in the ttydo
    // directory. If there are more than our initial capacity, we'll want to
    // allocate a larger array.
    char** list_paths = NULL;
    int list_count = count_saved_task_lists(&list_paths);

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
    TASKLIST_ARRAY_CAPACITY = list_cap;

    // use the capacity to create an appropriately-size array
    tasklists = calloc(TASKLIST_ARRAY_CAPACITY, sizeof(TaskList*));
    if (!tasklists)
    { return 1; }

    // iterate through the task list files and load them into memory
    for (int i = 0; i < list_count; i++)
    {
        // load the file, increase the array length, and free the path string
        tasklists[i] = load_task_list(list_paths[i]);
        TASKLIST_ARRAY_LENGTH++;
        free(list_paths[i]);
    }
    free(list_paths);

    return 0;
}

void tasklist_array_free()
{
    // iterate up to TASKLIST_ARRAY_LENGTH times and free each tasklist
    for (int i = 0; i < TASKLIST_ARRAY_LENGTH; i++)
    { task_list_free(tasklists[i]); }

    // free the tasklist pointer itself and set it back to NULL
    free(tasklists);
    tasklists = NULL;
}
