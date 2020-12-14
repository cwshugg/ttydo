// A module that defines utility functions for the command-line interface.
//
//      Connor Shugg

#ifndef UTILS_H
#define UTILS_H

// Module inclusions
#include "../visual/box.h"

// ========================= Error/Exit Functions ========================== //
// Helper function that is used to print an error message to stderr, then exit
// the program, due to some internal "fatal" error. If no message is provided,
// the program will still exit. The given exit code is used when exiting the
// program.
void fatality(int exit_code, char* message);

// Standard, run-of-the-mill "exit and clean up" function.
void finish();


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


// ======================= Task List Array Functions ======================= //
// Initializes an array of TaskList* pointers and saves it to the global
// task list array. Returns 0 on success, and a non-zero value on failure.
int tasklist_array_init();

// Frees the memory associated with the task list array.
void tasklist_array_free();

#endif
