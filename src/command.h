// This header file defines the struct(s) and functions that represent a single
// ttydo command, such as "add", or "remove".
//
//      Connor Shugg

#ifndef COMMAND_H
#define COMMAND_H


// ============================ Command Struct ============================= //
typedef struct _Command
{
    char* name;                  // name of the command (such as "Add")
    char* description;           // description of the command
    char* shorthand;             // shorthand command identifier (such as "a")
    char* longhand;              // longhand command identifier (such as "add")
    int (*handler)(char** args); // handler function for the command
} Command;

// Takes in parameters to fill in all the fields of a new command struct and
// attempts to create a new dynamically-allocated command. Returns the command
// on success and NULL on failure.
Command* command_new(char* n, char* d, char* s, char* l, int (*h)(char** args));

// Takes in a command pointer and attempts to free all of its memory.
void command_free(Command* comm);

// Takes in a command pointer and attempts to print out a formatted help
// message on how the command is used.
void command_print(Command* comm);

#endif
