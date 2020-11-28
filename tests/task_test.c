#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../src/task.h"
#include "../src/visual/box.h"

int main()
{
    Task* t1 = task_new("Task 1's Title", "Task 1's Description");
    char* t1_string = task_to_string(t1);
    printf("task with title and description:\n%s\n", t1_string);
    free(t1_string);

    Box* t1_box = box_new(0, 0, t1->title, t1->description);
    box_adjust_to_text(t1_box, 0);
    box_print(t1_box);

    box_free(t1_box);
    task_free(t1);

    // test task IDs
    t1 = task_new("TEST 1", "TEST 1");
    printf("Task ID of '%s': %lu\n", t1->title, t1->id);
    task_free(t1);
    
    t1 = task_new("TEST 2", "TEST 2");
    printf("Task ID of '%s': %lu\n", t1->title, t1->id);
    task_free(t1);
    
    t1 = task_new("TEST 3", "TEST 3");
    printf("Task ID of '%s': %lu\n", t1->title, t1->id);
    task_free(t1);
    
    t1 = task_new("TEST 4", "abcdefghijklmnopqrstuvwxyz");
    printf("Task ID of '%s': %lu\n", t1->title, t1->id);
    task_free(t1);
    
   t1 = task_new("TEST 5", NULL);
   printf("Task ID of '%s': %lu\n", t1->title, t1->id);
   task_free(t1);
    
   t1 = task_new("TEST 6", NULL);
   printf("Task ID of '%s': %lu\n", t1->title, t1->id);
   task_free(t1);

    return 0;
}