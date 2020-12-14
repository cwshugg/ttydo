// Implements the utility function definitions from utils.h
//
//      Connor Shugg

// Module inclusions
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "command.h"

// ======================= Globals/Macros/Prototypes ======================= //
// Globals
extern int NUM_COMMANDS;    // number of commands
extern Command** commands;  // dynamically-allocated command array
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
