#include <string.h>
#include "../src/scribe.h"
#include "../src/visual/terminal.h"

void remove_home_dir()
{
    char command[256];
    sprintf(command, "rm -rf ~/.ttydo");
    int result = system(command);
    printf("Home directory delete result: %d\n", result);
}

void test_scribe_string_to_task()
{
    // test a normal task string
    char* string1 = "11111111111111,0,TITLE TEST,DESCRIPTION TEST";
    Task* task1 = task_new_from_scribe_string(string1);
    if (task1) { task_free(task1); }
    
    // test a task string that's too long
    char string2[1024] = {'\0'};
    sprintf(string2, "12345678,1,THIS TITLE IS TOO LONG! IT'LL GET CUT OFF... HOPEFULLY,");
    for (int i = 0; i < 600; i++)
    { strncat(string2, "X", 1); }
    Task* task2 = task_new_from_scribe_string(string2);
    if (task2) { task_free(task2); }

    // test a string that only has an ID and 'is_complete' flag
    char* string3 = "222222,1";
    Task* task3 = task_new_from_scribe_string(string3);
    if (task3) { task_free(task3); }

    // test a string without a description
    char* string4 = "333333,0,title";
    Task* task4 = task_new_from_scribe_string(string4);
    if (task4) { task_free(task4); }

    // test a string with empty fields
    char* string5 = "444444,0,,";
    Task* task5 = task_new_from_scribe_string(string5);
    if (task5) { task_free(task5); }

    // test a string with ONLY an ID
    char* string6 = "5555555";
    Task* task6 = task_new_from_scribe_string(string6);
    if (task6) { task_free(task6); }

    // test a string with ONLY an ID and commas
    char* string7 = "666666,,,";
    Task* task7 = task_new_from_scribe_string(string7);
    if (task7) { task_free(task7); }

    // test a string with ONLY commas
    char* string8 = ",,,";
    Task* task8 = task_new_from_scribe_string(string8);
    if (task8) { task_free(task8); }
}

int main()
{
    // create a new task list
    TaskList* l1 = task_list_new("TEST LIST");
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
    // --------------------------------------------- //
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
    // --------------------------------------------- //

    // perform more tests with task-->string conversion
    test_scribe_string_to_task();

    // try creating a header string and converting back
    char* header = task_list_get_scribe_string(l1);
    TaskList* from_header = task_list_new_from_scribe_string(header);
    
    if (header) { free(header); }
    if (from_header) { task_list_free(from_header); }

    // attempt to save the list
    int result = save_task_list(l1);
    printf("\nSave result for task list '%s': %d\n", l1->name, result);

    // re-load the task list
    TaskList* loaded_list = load_task_list(l1->name);
    if (loaded_list)
    {
        BoxStack* bs = task_list_to_box_stack(loaded_list, get_terminal_width());
        printf("Loaded task list:\n");
        box_stack_print(bs);
        box_stack_free(bs);
    }

    // delete the task list
    int del_result = delete_task_list(loaded_list);
    printf("Delete result: %d\n", del_result);
    
    // free the task list(s)
    task_list_free(l1);
    if (loaded_list) { task_list_free(loaded_list); }

    // delete home directory
    //remove_home_dir();
}
