#include <stdlib.h>
#include <stdio.h>
#include "../src/cli/command.h"

// Sample command handler
int handler(char** args)
{
    printf("Command handler!\n");
    return 0;
}

int main()
{
    Command* c1 = command_new("Fart",
        "Makes your computer fart. Takes in a single argument.\nUsage: \"f/fart\"",
        "f", "fart", handler);
    printf("New command:\n-----\n");
    command_print(c1);
    printf("-----\n");

    // run the command's handler
    c1->handler(NULL);

    // free the command
    command_free(c1);
}
