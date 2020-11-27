// This module implements the definitions in tasklist.h.
//
//      Connor Shugg

// Module inclusions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tasklist.h"
#include "visual/terminal.h"
#include "visual/bar.h"


// =========================== List Elem Struct ============================ //
TaskListElem* task_list_elem_new(Task* payload, TaskListElem* previous, TaskListElem* next)
{
    // attempt to allocate a new list elem
    TaskListElem* elem = calloc(1, sizeof(TaskListElem));
    if (!elem) { return NULL; }

    // set the elem's fields and return
    elem->task = payload;
    elem->prev = previous;
    elem->next = next;
    return elem;
}

Task* task_list_elem_free(TaskListElem* elem)
{
    // if we were given a NULL pointer, return NULL
    if (!elem) { return NULL; }

    // pull out the payload, free the elem, and return the task
    Task* payload = elem->task;
    free(elem);
    return payload;
}


// ============================== List Struct ============================== //
TaskList* task_list_new(char* list_name)
{
    // if the given name is NULL, return NULL
    if (!list_name) { return NULL; }

    // attempt to allocate a new TaskList struct
    TaskList* list = calloc(1, sizeof(TaskList));
    if (!list) { return NULL; }

    // determine the correct length for the name string
    int name_length = strlen(list_name);
    if (name_length > TASK_LIST_NAME_MAX_LENGTH)
    { name_length = TASK_LIST_NAME_MAX_LENGTH; }
    // allocate the name string - if it fails, free and return NULL
    list->name = calloc(name_length + 1, sizeof(char));
    if (!list->name)
    {
        free(list);
        return NULL;
    }
    // copy the name into the list's field
    strncpy(list->name, list_name, name_length);

    // initialize the head, tail, and size to be NULL and 0
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void task_list_free(TaskList* list)
{
    // if we were given a NULL pointer, simply return
    if (!list) { return; }

    // if the size is greater than zero, free each individual element
    if (list->size > 0)
    {
        // start at the head node and iterate down, freeing elements
        TaskListElem* current = list->head;
        int i = 0;
        while (i < list->size && current)
        {
            TaskListElem* next = current->next;             // grab next elem
            Task* payload = task_list_elem_free(current);   // free current
            task_free(payload);                             // free task

            // increment counter and pointer
            i++;
            current = next;
        }
    }

    // free the name string and the list itself
    free(list->name);
    free(list);
}

int task_list_append(TaskList* list, Task* task)
{
    // if we were given a NULL Task pointer or NULL list pointer, return a
    // non-zero value to indicate failure
    if (!list || !task) { return 1; }

    // create a new list element to carry the Task. If allocation fails, return
    // a non-zero
    TaskListElem* elem = task_list_elem_new(task, list->tail, NULL);
    if (!elem) { return 1; }

    // if the list's size is currently zero, set both the head and tail to be
    // pointing at the new list element
    if (list->size == 0)
    {
        list->head = elem;
        list->tail = elem;
    }
    // otherwise, update the current tail node, and the list's tail pointer
    else
    {
        list->tail->next = elem;
        list->tail = elem;
    }
    
    // increment the list size and return
    list->size++;
    return 0;
}

Task* task_list_get_by_index(TaskList* list, int list_index)
{
    // if a NULL list pointer was given, return NULL
    if (!list) { return NULL; }
    // if an invalid list index was given, return NULL
    if (list_index < 0 || list_index >= list->size)
    { return NULL; }
    // if our list's size is 0, return NULL
    if (list->size == 0) { return NULL; }

    // iterate until we reach the correct list index
    TaskListElem* current = list->head;
    int i = 0;
    while (i++ < list_index && current)
    { current = current->next; }

    // extract the Task payload and return it
    return current->task;
}

Task* task_list_get_by_title(TaskList* list, char* task_title)
{
    // if NULL pointers were given, return NULL
    if (!list || !task_title) { return NULL; }
    // if our list's size is 0, return NULL
    if (list->size == 0) { return NULL; }

    // iterate through the list until a matching task is found
    TaskListElem* match = NULL;
    TaskListElem* current = list->head;
    int i = 0;
    while (i++ < list->size && current && !match)
    {
        // if the current task's title matches, we've found the correct element
        if (!strcmp(current->task->title, task_title))
        { match = current; }

        // increment pointer
        current = current->next;
    }

    // if a match wasn't found, return NULL
    if (!match) { return NULL; }

    // extract the Task and return it
    return match->task;
}

Task* task_list_remove(TaskList* list, Task* task)
{
    // if we were given NULL pointers, return a non-zero to indicate failure
    if (!list || !task) { return NULL; }

    // iterate through each list element to find the correct TaskListElem
    TaskListElem* match = NULL;
    TaskListElem* current = list->head;
    int i = 0;
    while (i++ < list->size && current && !match)
    {
        // if the pointers match, we've found the correct element
        if (current->task == task)
        { match = current; }

        // increment pointer
        current = current->next;
    }

    // if a match wasn't found, return NULL
    if (!match) { return NULL; }

    // get references to adjacent nodes
    TaskListElem* left = match->prev;
    TaskListElem* right = match->next;
    // update both pointers to point at each other, removing references to
    // our node-of-interest
    if (left) { left->next = right; }
    if (right) { right->prev = left; }
    // update list pointers (if needed)
    if (list->head == match) { list->head = right; }
    if (list->tail == match) { list->tail = left; }
    
    // decrement size, free the list elem and return the inner Task
    list->size--;
    Task* payload = task_list_elem_free(match);
    return payload;
}

BoxStack* task_list_to_box_stack(TaskList* list, int fill_width)
{
    // if we were given a NULL pointer, return NULL
    if (!list) { return NULL; }

    // first, we'll count the amount of space we'll need for our inner box
    // string - by summing up each task's to_string() result. We'll also use
    // this loop to count the number of completed tasks.
    int tasks_complete = 0;
    char* task_strings[list->size + 1];
    task_strings[list->size] = NULL; // null terminated
    TaskListElem* current = list->head;
    int i = 0;
    int box_string_size = 0;
    while (i < list->size && current)
    {
        tasks_complete += current->task->is_complete != 0;
        // convert the current task to a string and add the string's length
        task_strings[i] = task_to_string(current->task);
        if (task_strings[i]) { box_string_size += strlen(task_strings[i++]); }
        current = current->next;
    }
    // calculate the final size for our inner box string and allocate it
    box_string_size += (8 * list->size);
    char* box_string = calloc(box_string_size + 1, sizeof(char));
    // if the allocation failed, return NULL
    if (!box_string) { return NULL; }

    // add each task string to the box string (and free the individual task
    // strings along the way)
    for (int i = 0; i < list->size; i++)
    {
        strncat(box_string, task_strings[i], strlen(task_strings[i]));
        if (i < list->size - 1) { strncat(box_string, "\n", 1); }
        free(task_strings[i]);
    }

    // create a box stack to hold the multiple boxes. If allocation failed,
    // free all memory and return NULL
    BoxStack* stack = box_stack_new(2, 0);
    if (!stack)
    {
        free(box_string);
        return NULL;
    }
    
    // modify the first box - this will hold the tasks' text
    Box* text_box = stack->boxes[0];
    text_box->title = strdup(list->name);
    text_box->text = box_string;
    box_adjust_to_text(text_box, 1);

    // calculate the percentage of tasks completed and the width of the
    // progress bar.
    float percent_complete = (float) tasks_complete / (float) list->size;
    int progbar_width = text_box->width - 4;
    if (fill_width) { progbar_width = get_terminal_width() - 4; }
    // create a new progress bar
    ProgressBar* bar = progress_bar_new(progbar_width, percent_complete);

    // modify the second box - this will hold a progress bar
    Box* progress_box = stack->boxes[1];
    progress_box->title = strdup("Progress");
    progress_box->text = progress_bar_to_string(bar);
    box_adjust_to_text(progress_box, 1);

    // free the progress bar
    progress_bar_free(bar);

    // use the first box's width to set the entire stack's width. Then, if
    // 'fill_width' is set, adjuts the stack to fill the terminal
    box_stack_set_width(stack, text_box->width);
    if (fill_width) { box_stack_set_width(stack, get_terminal_width()); }

    // return the stack
    return stack;
}


// ================================ Testing ================================ //
#include <time.h>
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

    // create an array of integers from 0..(task_count - 1). Then, shuffle it
    int indexes[task_count];
    for (int i = 0; i < task_count; i++) { indexes[i] = i; }
    shuffle_int_array(indexes, task_count);

    // use the randomized indexes to remove elements from the list
    for (int i = 0; i < task_count; i++)
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
