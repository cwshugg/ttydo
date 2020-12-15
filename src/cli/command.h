// This header file defines the struct(s) and functions that represent a single
// ttydo command, such as "add", or "remove".
// Each command has a handler function that takes in a char** of command-line
// arguments and returns an integer - 0 on success, and non-zero on failure.
//
//      Connor Shugg

#ifndef COMMAND_H
#define COMMAND_H


// ============================ Command Struct ============================= //
typedef struct _Command
{
    char* name;                 // name of the command (such as "Add")
    char* description;          // description of the command
    char* shorthand;            // shorthand command identifier (such as "a")
    char* longhand;             // longhand command identifier (such as "add")
    // handler function for the command
    int (*handler)(struct _Command* comm, int argc, char** args);
    struct _Command** subcommands; // Array of sub-commands
    int subcommands_length;     // Number of sub-commands
} Command;

// Takes in parameters to fill in all the fields of a new command struct and
// attempts to create a new dynamically-allocated command. Returns the command
// on success and NULL on failure.
Command* command_new(char* n, char* s, char* l, char* d,
                     int (*h)(Command* comm, int argc, char** args));

// Takes in a command pointer and attempts to free all of its memory.
void command_free(Command* comm);

// Takes in a command pointer and a number of subcommands, and attempts to
// allocate memory to store the given number of subcommand Command structs.
// Returns 0 on success and a non-zero value on error.
int command_init_subcommands(Command* comm, int num_sub_commands);

// Takes in a user-inputted string representing a shorthand or longhand
// command identifier, and tries to match the given command with it. Returns
// 1 if it matches, and 0 if it doesn't match.
int command_match(Command* comm, char* string);

// Takes in a command pointer and attempts to print out a formatted help
// message on how the command is used.
void command_print(Command* comm);

// Takes a command pointer and attempts to write out its details to a single-
// -line string. The string is returned on success, and NULL on error.
char* command_to_string(Command* comm);

#endif
