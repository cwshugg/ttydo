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
    Command* c1 = command_new("Command name", "This is the command's description.",
                              "c", "command", handler);
    printf("New command:\n");
    command_print(c1);

    // free the command
    command_free(c1);
}
