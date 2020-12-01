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
const char* TTYDO_LIST_SUFFIX = ".tasklist";
const int TTYDO_HOME_DIR_LENGTH = 1024;
char TTYDO_HOME_DIR[TTYDO_HOME_DIR_LENGTH] = {'\0'}; // holds the home path
// Function prototypes
char* get_home_directory();
char* make_task_list_file_path(char* name);
char* format_string_for_file_name(char* string, int string_length);


// ======================== Header Implementations ========================= //
// Takes in a pointer to a TaskList and attempts to write it out to disk.
// Returns 0 on success and a non-zero value on failure.
int save_task_list(TaskList* list)
{
    // if we were given a NULL pointer, return a non-zero value
    if (!list) { return 1; }

    // first, we'll get a path to the file we'll write to
    char* file_path = make_task_list_file_path(list->name);
    if (!file_path) { return 1; }
    printf("File Path: '%s'\n", file_path);

    // get the header string from the task list
    char* header = task_list_get_scribe_string(list);
    printf("Header string: '%s'\n", header);

    // iterate through the task list

    // free memory
    free(header);
    free(file_path);
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
    // check to see if the home directory has already been initialized.
    // if so, simply return the string and avoid the expensive operations below
    if (strlen(TTYDO_HOME_DIR))
    { return TTYDO_HOME_DIR; }

    // retrieve the environment variable. If it fails, return NULL
    char* home = getenv("HOME");
    if (!home) { return NULL; }
    int home_length = strlen(home);
    
    // make and get the length of the .ttydo folder string
    int ttydo_length = strlen(TTYDO_FOLDER);

    // ensure TTYDO_HOME_DIR is zeroed out
    memset(TTYDO_HOME_DIR, 0, TTYDO_HOME_DIR_LENGTH);
    strncpy(TTYDO_HOME_DIR, home, home_length);
    strncat(TTYDO_HOME_DIR, "/", 1);
    strncat(TTYDO_HOME_DIR, TTYDO_FOLDER, ttydo_length);

    // use the string to check to see if the directory exists
    struct stat home_stats;
    if (stat(TTYDO_HOME_DIR, &home_stats) || !S_ISDIR(home_stats.st_mode))
    {
        // attempt to create the directory
        if (mkdir(TTYDO_HOME_DIR, 0664) == -1)
        {
            fprintf(stderr, "Internal error: couldn't create home directory: '%s'."
                    " Try creating the directory manually.\n", TTYDO_HOME_DIR);
        }
    }

    return TTYDO_HOME_DIR;
}

// Takes in the name of a task list and creates a file to which it will be
// saved to and restored from. The returned string is dynamically allocated.
char* make_task_list_file_path(char* name)
{
    // check for a NULL pointer
    if (!name) { return NULL; }

    // calculate the correct length to use for the name
    int name_length = strlen(name);
    if (name_length > TASK_LIST_NAME_MAX_LENGTH)
    { name_length = TASK_LIST_NAME_MAX_LENGTH; }
    // make the string passable as a file name
    char* fixed_name = format_string_for_file_name(name, name_length);
    
    // retrieve the home directory
    char* home = get_home_directory();
    if (!home)
    {
        fprintf(stderr, "Internal error: couldn't get the ttydo home directory.\n");
        return NULL;
    }
    int home_length = strlen(home);

    // allocate a new string of the appropriate length
    int suffix_length = strlen(TTYDO_LIST_SUFFIX);
    char* result = calloc(name_length + suffix_length + home_length + 8,
                          sizeof(char));
    
    // copy in the correct fields and return
    strncat(result, home, home_length);
    strncat(result, "/", 1);
    strncat(result, fixed_name, name_length);
    strncat(result, TTYDO_LIST_SUFFIX, suffix_length);

    // free memory and return
    free(fixed_name);
    return result;
}

// Takes in a string and its length and creates a new dynamically-allocated
// string containing a file-name-friendly version of the string
char* format_string_for_file_name(char* string, int string_length)
{
    // check for NULL pointer or 0 length
    if (!string || !string_length) { return NULL; }

    // allocate a new string
    char* result = calloc(string_length + 1, sizeof(char));
    if (!result) { return NULL; }

    // iterate through the string to build the new one
    for (int i = 0; i < string_length; i++)
    {
        if (string[i] == ' ' || string[i] == '\t' || string[i] == '\n' ||
            string[i] == '\r' || string[i] == '\v' || string[i] == '\f' ||
            string[i] == '/' || string[i] == '\\')
        { result[i] = '_'; }
        else
        { result[i] = string[i]; }
    }

    // return the resulting string
    return result;
}
