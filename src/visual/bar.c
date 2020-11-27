// A module that implements bar.h's definitions.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "bar.h"

// ========================== Progress Bar Struct ========================== //



// ================================ Testing ================================ //
#include <string.h>
int main()
{
    printf("Test progress bar:\n\t");
    printf("%s", PROGBAR_L_BORDER);
    for (int i = 0; i < 32; i++)
    { printf("%s", PROGBAR_FILLED); }
    for (int i = 0; i < 12; i++)
    { printf("%s", PROGBAR_EMPTY); }
    printf("%s\n", PROGBAR_R_BORDER);

    char* x = PROGBAR_FILLED;
    printf("Length of one extended char: %lu\n", strlen(x));
}
