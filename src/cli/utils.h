// A module that defines utility functions for the command-line interface.
//
//      Connor Shugg

// ========================= Error/Exit Functions ========================== //
// Helper function that is used to print an error message to stderr, then exit
// the program, due to some internal "fatal" error. If no message is provided,
// the program will still exit. The given exit code is used when exiting the
// program.
void fatality(int exit_code, char* message);

// Standard, run-of-the-mill "exit and clean up" function.
void finish();


// ========================== Printing Functions =========================== //
// Prints an 'intro' page that's displayed when the user executes ttydo without
// any command-line arguments.
void print_intro();

// Prints an ascii/box art logo for ttydo. Takes in a 'prefix' string that's
// printed before each line of the logo.
void print_logo(char* prefix);
