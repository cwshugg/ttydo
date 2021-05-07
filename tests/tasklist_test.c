#include <time.h>
#include "../src/tasklist.h"

void shuffle_int_array(int* array, int length)
{
    srand(time(NULL));
    for (int i = length - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        // swap two elements
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int main()
{
    // testing a single list element
    Task* t1 = task_new("Task 1", "This is task 1!");
    TaskListElem* e1 = task_list_elem_new(t1, NULL, NULL);
    Task* e1_task = task_list_elem_free(e1);
    printf("Task pointers equal? %p == %p\n\n", t1, e1_task);
    task_free(t1);

    // testing a list
    TaskList* l1 = task_list_new("list 1");
    printf("Testing Task List: %s\n", l1->name);

    // inserting Tasks into the list
    int task_count = 5;
    for (int i = 0; i < task_count; i++)
    {
        char task_name[16];
        snprintf(task_name, 16, "Task %d", i);
        char task_desc[64];
        snprintf(task_desc, 64, "This is task %d's description.", i);
        // create the task and insert it
        Task* task = task_new(task_name, task_desc);
        if (i % 2) { task->is_complete = 1; }
        int failed = task_list_append(l1, task);
        
        char* task_string = task_to_string(task);
        if (!failed)
        { printf("Successfully inserted: '%s'\n", task_string); }
        else
        { printf("Failed to insert task: '%s'\n", task_string); }
        if (task_string) { free(task_string); }
    }

    // search the list for every element (BY INDEX)
    for (int i = 0; i < task_count; i++)
    {
        Task* result = task_list_get_by_index(l1, i);
        printf("Search by index (%d): '%s'\n", i, result->title);
    }

    // search the list for every element (BY TITLE)
    for (int i = 0; i < task_count; i++)
    {
        char task_name[16];
        snprintf(task_name, 16, "Task %d", i);
        Task* result = task_list_get_by_title(l1, task_name);
        printf("Search by index ('%s'): '%s'\n", task_name, result->title);
    }

    // print the list in box stack form
    printf("Box print 1:\n");
    BoxStack* stack1 = task_list_to_box_stack(l1, 1);
    box_stack_print(stack1);
    box_stack_free(stack1);
    printf("Box print 2:\n");
    BoxStack* stack2 = task_list_to_box_stack(l1, 0);
    box_stack_print(stack2);
    box_stack_free(stack2);

    // INSERT AT HEAD
    Task* task_insert1 = task_new("Inserted Task 1", "DESCRIPTION");
    int failed = task_list_insert(l1, task_insert1, 0);
    if (failed) { fprintf(stderr, "FAILED TO INSERT\n"); }
    printf("-----\nAfter inserting at head:\nList head: %p\nList tail: %p\nList size: %d\n-----\n",
           l1->head, l1->tail, l1->size);
    stack1 = task_list_to_box_stack(l1, 1);
    box_stack_print(stack1);
    box_stack_free(stack1);

    // INSERT AT TAIL
    Task* task_insert2 = task_new("Inserted Task 2", "DESCRIPTION");
    failed = task_list_insert(l1, task_insert2, l1->size);
    if (failed) { fprintf(stderr, "FAILED TO INSERT\n"); }
    printf("-----\nAfter inserting at tail:\nList head: %p\nList tail: %p\nList size: %d\n-----\n",
           l1->head, l1->tail, l1->size);
    stack1 = task_list_to_box_stack(l1, 1);
    box_stack_print(stack1);
    box_stack_free(stack1);

    // INSERT IN THE MIDDLE
    Task* task_insert3 = task_new("Inserted Task 3", "DESCRIPTION");
    failed = task_list_insert(l1, task_insert3, 2);
    if (failed) { fprintf(stderr, "FAILED TO INSERT\n"); }
    printf("-----\nAfter inserting in middle:\nList head: %p\nList tail: %p\nList size: %d\n-----\n",
           l1->head, l1->tail, l1->size);
    stack1 = task_list_to_box_stack(l1, 1);
    box_stack_print(stack1);
    box_stack_free(stack1);

    // get an array of the tasks
    Task* task_array[l1->size];
    TaskListElem* elem = l1->head;
    int elem_index = 0;
    while (elem_index < l1->size && elem)
    {
        task_array[elem_index++] = elem->task;
        elem = elem->next;
    }

    // create an array of integers from 0..(task_count - 1). Then, shuffle it
    int list_size = l1->size;
    int indexes[list_size];
    for (int i = 0; i < list_size; i++) { indexes[i] = i; }
    shuffle_int_array(indexes, task_count);

    // use the randomized indexes to remove elements from the list
    for (int i = 0; i < list_size; i++)
    {
        int index = indexes[i];
        Task* result = task_list_remove(l1, task_array[index]);
        printf("Result of removing task: '%s' == '%s'", task_array[index]->title, result->title);
        printf("\tList: [head = %p] [tail = %p] [size = %d]\n", l1->head, l1->tail, l1->size);
        task_free(result);
    }
    printf("-----\nAfter removing all elements:\nList head: %p\nList tail: %p\nList size: %d\n-----\n",
           l1->head, l1->tail, l1->size);

    // free the entire list
    task_list_free(l1);
}
