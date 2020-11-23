// A module that implements terminal.h's definitions
//
//      Connor Shugg

#include "terminal.h"

// Helper function prototypes
struct winsize get_window_size_info();


// ======================== Header Implementations ========================= //
int get_terminal_width()
{ return (int) get_window_size_info().ws_col; }

int get_terminal_height()
{ return (int) get_window_size_info().ws_row; }


// =========================== Helper Functions ============================ //
struct winsize get_window_size_info()
{
    struct winsize window;
    ioctl(0, TIOCGWINSZ, &window);
    return window;
}
