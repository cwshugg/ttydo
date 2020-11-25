// A small C header file defining functions used to retrieve terminal-related
// information (such as the terminal size)
//
//      Connor Shugg

// https://stackoverflow.com/questions/1022957/getting-terminal-width-in-c

#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>

// Retrieves the width of the terminal window, in characters.
int get_terminal_width();

// Retrieves the height of the terminal window, in characters.
int get_terminal_height();

#endif
