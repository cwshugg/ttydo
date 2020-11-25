// A header file that defines structs and functions used to represent a box
// stack: a vertical stack of Box objects (defined in box.h). A box stack has
// the following properties:
//  - Every box in the stack has the same width
//  - Each box has its own title, text, and height
//  - When printed, each box's borders overlap with each other
//
//      Connor Shugg

#ifndef BOXSTACK_H
#define BOXSTACK_H

// Module inclusions
#include "box.h"

// =========================== Box Stack Struct ============================ //
typedef struct _BoxStack
{
    Box** boxes;        // the array of individual Box structs in the stack
    int size;           // the number of Box structs in the stack
    int width;          // the global width for every box
} BoxStack;

// Constructor: takes in the size of the box stack, and the global width for
// each box, and attempts to dynamically allocate and return a pointer to a
// BoxStack struct. On failure, NULL is returned.
BoxStack* box_stack_new(int stack_size, int box_width);

// Destructor: takes in a pointer to a dynamically-allocated BoxStack struct
// and attempts to free all memory associated with it.
void box_stack_free(BoxStack* stack);

// Takes in a width and applies it to all the boxes on the stack. If a width
// less than box.h's BOX_MIN_WIDTH is given, BOX_MIN_WIDTH will be used
// instead.
void box_stack_set_width(BoxStack* stack, int box_width);

// Takes in a pointer to a BoxStack and attempts to print each box on top of
// each other.
void box_stack_print(BoxStack* stack);

#endif
