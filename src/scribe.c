// This module implements scribe.h's definitions.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
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
int file_is_tasklist(char* path);


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
    // open the file with write permissions (free and return on failure)
    FILE* file = fopen(file_path, "w");
    if (!file)
    {
        free(file_path);
        if (errno) { return errno; }
        return 1;
    }

    // get the header string from the task list and write it out
    char* header = task_list_get_scribe_string(list);
    fprintf(file, "%s\n", header);

    // iterate through the task list
    TaskListElem* current = list->head;
    int i = 0;
    while (i++ < list->size && current)
    {
        // get a task string and write it out to the file
        char* task_string = task_get_scribe_string(current->task);
        fprintf(file, "%s\n", task_string);
        free(task_string);

        // increment pointer
        current = current->next;
    }

    // free memory and close the file pointer
    fclose(file);
    free(header);
    free(file_path);
    return 0;
}

// Takes in the name of a TaskList and attempts to load it in from disk.
// On success, a dynamically-allocated TaskList pointer is returned. Otherwise,
// NULL is returned.
TaskList* load_task_list(char* name)
{
    // check for a null parameter
    if (!name) { return NULL; }

    // using the name, we'll generate the path at which the file is stored
    char* file_path = make_task_list_file_path(name);
    if (!file_path) { return NULL; }
    // attempt to open the file with read permission
    FILE* file = fopen(file_path, "r");
    if (!file)
    {
        free(file_path);
        return NULL;
    }

    // determine a maximum line length to read
    size_t max_line_length = TASK_LIST_NAME_MAX_LENGTH + TASK_TITLE_MAX_LENGTH +
                             TASK_DESCRIPTION_MAX_LENGTH + 32;

    // read the first line - this should be the header string.
    // on failure, free all and return
    char* buffer = calloc(max_line_length + 1, sizeof(char));
    size_t read_amount = getline(&buffer, &max_line_length, file);
    if (read_amount <= 0)
    {
        free(file_path);
        free(buffer);
        fclose(file);
    }
    TaskList* list = task_list_new_from_scribe_string(buffer);

    // iterate through the remaining lines and interpret them as tasks
    while (getline(&buffer, &max_line_length, file) > 0)
    {
        // if there's a '\n' at the end of the string, replace it with
        // a string terminator
        char* newline = strstr(buffer, "\n");
        if (newline) { *newline = '\0'; }
        // attempt to convert the string into a Task object. If one was
        // successfully created, add it to the task list
        Task* task = task_new_from_scribe_string(buffer);
        if (task)
        { task_list_append(list, task); }
    }

    // close the file and free memory
    fclose(file);
    free(file_path);
    free(buffer);

    return list;
}

int count_saved_task_lists(char*** list_names)
{
    // get the ttydo home directory
    char* home = get_home_directory();
    if (!home)
    { return 1; }

    // attempt to open the home directory for reading
    DIR* dir = opendir(home);
    if (!dir)
    {
        free(home);
        return 1;
    }

    // allocate an array of strings and iterate through the array again to
    // copy each file name. We'll start with a set array capacity and realloc
    // if more space is needed.
    int names_capacity = 8;
    char** names = calloc(names_capacity, sizeof(char*));
    int tasklist_count = 0;
    struct dirent* de;
    while ((de = readdir(dir)) != NULL)
    {
        // we only care about the files that end in the correct prefix
        if (file_is_tasklist(de->d_name))
        {
            // if we're going to run out of space in our array, reallocate
            if (tasklist_count == names_capacity)
            {
                names_capacity <<= 1; // multiply by 2
                names = realloc(names, names_capacity * sizeof(char*));
            }

            // find the location of the '.tasklist' suffix (we know it has
            // one at this point, thanks to 'file_is_tasklist')
            char* suffix = strstr(de->d_name, TTYDO_LIST_SUFFIX);
            int length = strlen(de->d_name) - strlen(suffix);

            // allocate a new string
            names[tasklist_count] = calloc(length + 1, sizeof(char));
            // copy the file name for the tasklist
            strncat(names[tasklist_count], de->d_name, length);

            // increment the counter
            tasklist_count++;
        }
    }

    // close the directory
    closedir(dir);

    // set the given char*** to point at the created array, and return the
    // number of task lists that were counted
    *list_names = names;
    return tasklist_count;
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
        int make_result = mkdir(TTYDO_HOME_DIR, 0777);
        if (make_result == -1)
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

// Takes in a path to a file and determines if it ends in the correct suffix
// to be identified as a saved task list. Returns 1 if true, and 0 if false
// (or if the given pointer was NULL or empty)
int file_is_tasklist(char* path)
{
    if (!path) { return 0; }

    // compute the length of the path and the suffix
    int length = strlen(path);
    int suffix_length = strlen(TTYDO_LIST_SUFFIX);
    if (length < suffix_length) { return 0; }

    // search for the suffix in the path. If it's at the very end of the
    // string, we'll return true
    char* suffix_location = strstr(path, TTYDO_LIST_SUFFIX);
    return suffix_location &&
           suffix_location - path == length - suffix_length;
}
