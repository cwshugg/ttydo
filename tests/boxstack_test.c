#include <string.h>
#include "../src/visual/boxstack.h"

int main()
{
    // create a box stack
    BoxStack* stack = box_stack_new(4, 100);
    box_stack_set_width(stack, 50);
    printf("New box stack (%p):\n- Size = %d\n- Width = %d\n", stack,
           stack->size, stack->width);
    for (int i = 0; i < stack->size; i++)
    {
        char title[16];
        snprintf(title, 16, "Box %d", i);
        stack->boxes[i]->title = strdup(title);
        stack->boxes[i]->height = 3;

        printf("- Box %d: [width: %d] [height: %d] [title: '%s'] [text: '%s']\n",
               i, stack->boxes[i]->width, stack->boxes[i]->height,
               stack->boxes[i]->title, stack->boxes[i]->text);
    }

    printf("printing the box stack:\n");
    box_stack_print(stack);
    
    // free the box stack
    box_stack_free(stack);

    return 0;
}
