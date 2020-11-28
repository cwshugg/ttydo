#include "../src/scribe.h"

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
        Task* task = task_new(task_name, task_desc);
        if (i % 2) { task->is_complete = 1; }
        task_array[i] = task;
        int failed = task_list_append(l1, task);
        
        char* task_string = task_to_string(task);
        if (!failed)
        { printf("Successfully inserted: '%s'\n", task_string); }
        else
        { printf("Failed to insert task: '%s'\n", task_string); }
        if (task_string) { free(task_string); }
    }

    // attempt to save the list
    int result = save_task_list(l1);
    printf("\nSave result for task list '%s': %d\n", l1->name, result);
    
    // free the task list
    task_list_free(l1);
}
