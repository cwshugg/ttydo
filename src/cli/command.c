// This implements the functions defined in command.h.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command.h"

// ======================== Header Implementations ========================= //
Command* command_new(char* n, char* s, char* l, char* d, int (*h)(int argc, char** args))
{
    // check for null pointers
    if (!n || !d || !s || !l || !h)
    { return NULL; }

    // allocate a new struct - return NULL on failure
    Command* comm = calloc(1, sizeof(Command));
    if (!comm) { return NULL; }

    // copy each string
    comm->name = strdup(n);
    comm->description = strdup(d);
    comm->shorthand = strdup(s);
    comm->longhand = strdup(l);
    comm->handler = h;

    // check for failed string duplications
    if (!comm->name || !comm->description || !comm->shorthand || !comm->longhand)
    {
        command_free(comm);
        return NULL;
    }

    // return the created command
    return comm;
}

void command_free(Command* comm)
{
    // check for a null pointer
    if (!comm) { return; }

    // free each inner string
    if (comm->name) { free(comm->name); }
    if (comm->description) { free(comm->description); }
    if (comm->shorthand) { free(comm->shorthand); }
    if (comm->longhand) { free(comm->longhand); }

    // free the main pointer
    free(comm);
}

void command_print(Command* comm)
{
    // check for null parameter
    if (!comm) { return; }

    // print a simple string to represent the command
    printf("Command: %s ('%s' or '%s')\n%s\n", comm->name, comm->shorthand,
           comm->longhand, comm->description);
}

char* command_to_string(Command* comm)
{
    // check for null pointer
    if (!comm) { return NULL; }

    // print the command short/long hand into one string
    char left_side[128];
    sprintf(left_side, "%s/%s", comm->longhand, comm->shorthand);

    int cap = 256;
    char* result = calloc(cap + 1, sizeof(char));
    if (!result) { return NULL; }
    snprintf(result, cap, "%-16s %s", left_side, comm->description);

    // return the created string
    return result;
}
