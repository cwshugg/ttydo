#include <string.h>
#include "../src/scribe.h"

void remove_home_dir()
{
    char command[256];
    sprintf(command, "rm -rf ~/.ttydo");
    int result = system(command);
    printf("Home directory delete result: %d\n", result);
}

int main()
{
    // create a new task list
    TaskList* l1 = task_list_new("list 1");
    int task_count = 5;
    Task* task_array[task_count];
    for (int i = 0; i < task_count; i++)
    {
        char task_name[16];
        snprintf(task_name, 16, "Task %d", i);
        char task_desc[64];
        snprintf(task_desc, 64, "This is task %d's description.", i);
        // create the task and insert it
        Task* task = NULL;
        if (i == task_count - 1)
        { task = task_new(task_name, NULL); }
        else if (i == task_count - 2)
        {
            task = task_new(task_name, NULL);
            free(task->title);
            task->title = NULL;
        }
        else
        { task = task_new(task_name, task_desc); }
        
        if (i % 2) { task->is_complete = 1; }
        task_array[i] = task;
        int failed = task_list_append(l1, task);
        
        char* task_string = task_to_string(task);
        if (!failed)
        { printf("Successfully inserted: '%s'\n", task_string); }
        else
        { printf("Failed to insert task: '%s'\n", task_string); }
        if (task_string) { free(task_string); }

        // attempt to get a scribe string
        char* ss = task_get_scribe_string(task);
        printf("Scribe Task String: '%s'\n", ss);
        if (ss) { free(ss); }
    }
    
    // modify the first task to have a really long title and description
    Task* task0 = l1->head->task;
    // title
    int new_title_length = TASK_TITLE_MAX_LENGTH + 8;
    task0->title = realloc(task0->title, new_title_length + 1);
    memset(task0->title, 0, new_title_length + 1);
    for (int i = 0; i < new_title_length; i++)
    { task0->title[i] = 'X'; }
    // description
    int new_desc_length = TASK_DESCRIPTION_MAX_LENGTH + 8;
    task0->description = realloc(task0->description, new_desc_length + 1);
    memset(task0->description, 0, new_desc_length + 1);
    for (int i = 0; i < new_desc_length; i++)
    { task0->description[i] = 'Y'; }
    // generate a string for it to test the truncating
    char* ss = task_get_scribe_string(task0);
    printf("\nTruncated task string: '%s'\n", ss);
    if (ss) { free(ss); }

    // try creating a header string and converting back
    char* header = task_list_get_scribe_string(l1);
    TaskList* from_header = task_list_new_from_scribe_string(header);
    
    if (header) { free(header); }
    if (from_header) { task_list_free(from_header); }

    // attempt to save the list
    int result = save_task_list(l1);
    printf("\nSave result for task list '%s': %d\n", l1->name, result);
    
    // free the task list
    task_list_free(l1);

    // delete home directory
    remove_home_dir();
}


