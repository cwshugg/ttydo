#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../src/visual/bar.h"

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

    printf("\nprogress bar test 1:\n");
    ProgressBar* bar = progress_bar_new(107, 0.33);
    char* bar_string = progress_bar_to_string(bar);
    printf("%s\n", bar_string);
    free(bar_string);
    progress_bar_free(bar);
    
    printf("\nprogress bar test 2:\n");
    bar = progress_bar_new(107, 0.66);
    bar_string = progress_bar_to_string(bar);
    printf("%s\n", bar_string);
    free(bar_string);
    progress_bar_free(bar);
    
    printf("\nprogress bar test 3:\n");
    bar = progress_bar_new(107, 1.0);
    bar_string = progress_bar_to_string(bar);
    printf("%s\n", bar_string);
    free(bar_string);
    progress_bar_free(bar);
    
    printf("\nprogress bar test at minimum width:\n");
    bar = progress_bar_new(0, 1.0);
    bar_string = progress_bar_to_string(bar);
    printf("%s\n", bar_string);
    free(bar_string);
    progress_bar_free(bar);
}
