// This module implements scribe.h's definitions.
//
//      Connor Shugg

// Module inclusions
#include <string.h>
#include <sys/stat.h>
#include "scribe.h"

// =============== Constants and Helper Function Prototypes ================ //
const char* TTYDO_FOLDER = ".ttydo";
const char* TTYDO_LIST_FOLDER = "lists";
// Function prototypes
char* get_home_directory();


// ======================== Header Implementations ========================= //
// Takes in a pointer to a TaskList and attempts to write it out to disk.
// Returns 0 on success and a non-zero value on failure.
int save_task_list(TaskList* list)
{
    // if we were given a NULL pointer, return a non-zero value
    if (!list) { return 1; }
    // otherwise, we'll first retrieve the home directory
    char* home = get_home_directory();

    // get the header string from the task list
    char* header = task_list_get_scribe_string(list);
    printf("Header string: '%s'\n", header);

    // free memory
    free(header);
    free(home);
    return 0;
}

// Takes in the name of a TaskList and attempts to load it in from disk.
// On success, a dynamically-allocated TaskList pointer is returned. Otherwise,
// NULL is returned
TaskList* load_task_list(char* name)
{
    // TODO
    return NULL;
}


// =========================== Helper Functions ============================ //
// Uses the $HOME environment variable to build a string path to ttydo's home
// directory. (located at: ~/.ttydo/)
char* get_home_directory()
{
    // retrieve the environment variable. If it fails, return NULL
    char* home = getenv("HOME");
    if (!home) { return NULL; }
    int home_length = strlen(home);
    
    // make and get the length of the .ttydo folder string
    int ttydo_length = strlen(TTYDO_FOLDER);

    // allocate a new string
    char* result = calloc(home_length + ttydo_length + 2, sizeof(char));
    strncpy(result, home, home_length);
    strncat(result, "/", 1);
    strncat(result, TTYDO_FOLDER, ttydo_length);

    // use the string to check to see if the directory exists
    struct stat home_stats;
    if (stat(result, &home_stats) || !S_ISDIR(home_stats.st_mode))
    {
        // attempt to create the directory
        if (mkdir(result, 0664) == -1)
        {
            fprintf(stderr, "Internal error: couldn't create home directory: '%s'."
                    " Try creating the directory manually.\n", result);
        }
    }

    return result;
}
