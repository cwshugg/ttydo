// A module that implements boxstack.h's definitions.
//
//      Connor Shugg

// Module inclusions
#include "boxstack.h"

// =========================== Box Stack Struct ============================ //
// Constructor: takes in the size of the box stack, and the global width for
// each box, and attempts to dynamically allocate and return a pointer to a
// BoxStack struct. On failure, NULL is returned.
BoxStack* box_stack_new(int stack_size, int box_width)
{
    // if we were given invalid inputs, adjust or return NULL
    if (stack_size <= 0) { return NULL; }
    if (box_width < BOX_MIN_WIDTH) { box_width = BOX_MIN_WIDTH; }

    // allocate memory for a box stack. If it fails, return NULL
    BoxStack* stack = calloc(1, sizeof(BoxStack));
    if (!stack) { return NULL; }
    stack->size = stack_size;
    stack->width = box_width;

    // allocate the inner box array, then allocate each box
    stack->boxes = calloc(stack->size, sizeof(char*));
    for (int i = 0; i < stack->size; i++)
    {
        // allocate a box - if it fails, free all previous allocations and
        // return NULL
        stack->boxes[i] = box_new(stack->width, 0, NULL, NULL);
        if (!stack->boxes[i])
        {
            for (int j = 0; j < i; j++) { box_free(stack->boxes[i]); }
            free(stack);
            return NULL;
        }
    }

    // return the stack's pointer
    return stack;
}

void box_stack_free(BoxStack* stack)
{
    // if we were given a NULL pointer, do nothing
    if (!stack) { return; }

    // free each individual box, then free the inner box array pointer
    for (int i = 0; i < stack->size; i++)
    { box_free(stack->boxes[i]); }
    free(stack->boxes);

    // free the main pointer
    free(stack);
}

void box_stack_set_width(BoxStack* stack, int box_width)
{
    // if we were given a NULL pointer, return
    if (!stack) { return; }
    // if the given width is below the minimum, use the minimum value instead
    if (box_width < BOX_MIN_WIDTH) { box_width = BOX_MIN_WIDTH; }

    // set the widths for all boxes in the stack
    stack->width = box_width;
    for (int i = 0; i < stack->size; i++)
    { stack->boxes[i]->width = box_width; }
}

void box_stack_print(BoxStack* stack)
{
    
}


// ================================ Testing ================================ //
// int main()
// {
//     // create a box stack
//     BoxStack* stack = box_stack_new(4, 100);
//     box_stack_set_width(stack, 316);
//     printf("New box stack (%p):\n- Size = %d\n- Width = %d\n", stack,
//            stack->size, stack->width);
//     for (int i = 0; i < stack->size; i++)
//     {
//         printf("- Box %d: [width: %d] [height: %d] [title: '%s'] [text: '%s']\n",
//                i, stack->boxes[i]->width, stack->boxes[i]->height,
//                stack->boxes[i]->title, stack->boxes[i]->text);
//     }
    
//     // free the box stack
//     box_stack_free(stack);

//     return 0;
// }
