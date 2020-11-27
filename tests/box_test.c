#include <string.h>
#include "../src/visual/box.h"
#include "../src/visual/terminal.h"

void free_null_terminated_string_array(char** lines)
{
    // iterate until NULL is reached (there should be an extra slot at the end
    // of the array that's been NULL'd out)
    char** current = lines;
    while (*current)
    { free(*(current++)); }

    // free the array pointer
    free(lines);
}

int main()
{
    printf("box with no title/text:\n");
    Box* b1 = box_new(64, 4, NULL, NULL);
    box_print(b1);
    box_free(b1);
    
    printf("box with title:\n");
    Box* b2 = box_new(64, 4, "title testing", NULL);
    box_print(b2);
    box_free(b2);

    printf("box with runoff title:\n");
    Box* b2_1 = box_new(32, 4, "this title has runoff, see? watch this", NULL);
    box_print(b2_1);
    box_free(b2_1);
    
    printf("box with title and text:\n");
    Box* b3 = box_new(64, 4, "title testing", "this is the box's text. Just one line.");
    box_print(b3);
    box_free(b3);
    
    printf("box with title and text that's too long:\n");
    Box* b4 = box_new(32, 4, "title testing", "this box has text that is too long for it.");
    box_print(b4);
    box_free(b4);
    
    printf("box with title and just enough lines of text:\n");
    Box* b5 = box_new(48, 4, "title testing", "this is the first line of text\nthis is the second");
    box_print(b5);
    box_free(b5);
    
    printf("box with title and too many lines of text:\n");
    Box* b6 = box_new(64, 4, "title testing", "text line 1\ntext line 2\ntext line 3");
    box_print(b6);
    box_free(b6);
    
    printf("box readjusted to fill text:\n");
    Box* b7 = box_new(1, 1, "title testing", "text line 1 ABCDEFGHIJKLMNOP\ntext line 2\ntext line 3");
    box_adjust_to_text(b7, 0);
    box_print(b7);
    box_free(b7);
    
    printf("box filling the terminal width:\n");
    Box* b8 = box_new(get_terminal_width(), 1,
                      "Box as long as the terminal window",
                      "line1\nline2\nline3\nline4\nline5");
    box_adjust_to_text(b8, 0);
    box_print(b8);

    // get the character array and manually print
    printf("printing the box manually:\n");
    char** b8_lines = box_to_lines(b8);
    for (int i = 0; i < b8->height; i++)
    { printf("%s\n", b8_lines[i]); }
    free_null_terminated_string_array(b8_lines);
    box_free(b8);

    return 0;
}