// A module that defines utility functions for the command-line interface.
//
//      Connor Shugg

#ifndef UTILS_H
#define UTILS_H

// Module inclusions
#include "command.h"
#include "../visual/box.h"
#include "../tasklist.h"

// ================================ Macros ================================= //
#define H_LINE "\u2500" // used for various prints in the CLI


// ========================= Error/Exit Functions ========================== //
// Helper function that is used to print an error message to stderr, then exit
// the program, due to some internal "fatal" error. If no message is provided,
// the program will still exit. The given exit code is used when exiting the
// program.
void fatality(int exit_code, char* message);

// Standard, run-of-the-mill "exit and clean up" function.
void finish();

// A printf-like function that prints to STDERR. Used to print error messages.
void eprintf(const char* format, ...);


// ========================== Printing Functions =========================== //
// Prints an 'intro' page that's displayed when the user executes ttydo without
// any command-line arguments.
void print_intro();

// Prints an ascii/box art logo for ttydo. Takes in a 'prefix' string that's
// printed before each line of the logo.
void print_logo(char* prefix);

// Takes in title/text and attempts to print it out. If the box's width is equal
// to the terminal width, and the text is still too long, the box instead has
// just the top and bottom lines printed, so the text can wrap around on the
// terminal.
void print_box_terminal_safe(char* title, char* text);

// Takes in a certain length and prints out 'length' number of H_LINE
// characters to the terminal.
void print_horizontal_line(int length);

// Takes in a command and attempts to print out its subcommands in a box.
// If the given parameter is NULL, the global list of commands is used instead.
int print_subcommands(Command* comm, char* title);


// ======================= Task List Array Functions ======================= //
// Initializes an array of TaskList* pointers and saves it to the global
// task list array. Returns 0 on success, and a non-zero value on failure.
int tasklist_array_init();

// Frees the memory associated with the task list array.
void tasklist_array_free();

// Takes in a TaskList pointer and attempts to add it to the global array.
// Returns 0 on success and a non-zero value on failure.
int tasklist_array_add(TaskList* list);

// Takes in a TaskList pointer and attempts to remove it from the global array.
// List members that occur after the removed entry are shifted down.
// Returns 0 on success and a non-zero value on error.
int tasklist_array_remove(int index);

// Takes in a string input from the command-line and attempts to interpret it
// as either a number or the name of a task list. In either case, the index
// of the matching list is returned, or -1 if it can't be found.
// Since this comes from user input, if a number is parsed, it gets decreased
// by one to make an array index (1 --> 0, 2, --> 1, etc.)
int tasklist_array_find(char* input);


// ======================== Other Helper Functions ========================= //
// Sorts an array of strings using 'sort_string_array_cmp' as the comparison
// function.
void sort_string_array(const char* strings[], int length);

// The comparison function for sorting an array of strings
int sort_string_array_cmp(const void* a, const void* b);

#endif
