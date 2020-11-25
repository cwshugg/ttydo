// A module that implements boxstack.h's definitions.
//
//      Connor Shugg

// Module inclusions
#include <string.h>
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
    // if we were given a NULL pointer, or the box's size is zero, return
    if (!stack || stack->size == 0) { return; }

    // iterate through each box
    for (int i = 0; i < stack->size; i++)
    {
        Box* b = stack->boxes[i];
        // retrieve an array of lines to print the current box
        char** lines = box_to_lines(b);

        // if this isn't the last box in the stack, we DON'T want to print the
        // last line for this box. That line will be replaced by the top line
        // of the *next* box.
        int end = b->height;
        if (i < stack->size - 1) { end--; }
        // iterate through the lines and print
        for (int j = 0; j < end; j++)
        {
            // if this is the top line of the box, AND it's not the first box,
            // we want to modify the corner characters to visually connect the
            // boxes.
            if (i > 0 && j == 0)
            {
                strncpy(lines[j], BOX_L_CROSS, strlen(BOX_L_CROSS));
                strncpy(lines[j] + strlen(lines[j]) - strlen(BOX_TR_CORNER),
                        BOX_R_CROSS, strlen(BOX_R_CROSS));
            }
            fprintf(stdout, "%s\n", lines[j]);
        }

        // free the line strings
        char** current = lines;
        while (*current) { free(*(current++)); }
        free (lines);
    }
}


// ================================ Testing ================================ //
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
