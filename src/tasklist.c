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
#include "cli/utils.h"


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
    snprintf(list->name, name_length + 1, "%s", list_name);

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

int task_list_insert(TaskList* list, Task* task, int index)
{
    // check for null pointers
    if (!list || !task) { return 1; }
    // check for an out-of-bounds index
    if (index < 0 || index > list->size)
    { return 1; }

    // if the list is empty OR the index specified is the final index, just
    // append the task and return
    if (list->size == 0 || index == list->size)
    { return task_list_append(list, task); }

    // iterate and find the location of the new task
    TaskListElem* insert_before = list->head;
    int i = 0;
    while (i++ < index && insert_before)
    { insert_before = insert_before->next; }

    // create a new list element to carry the task (make sure to specify that
    // it will be placed before the node we just found)
    TaskListElem* elem = task_list_elem_new(task, insert_before->prev,
                                            insert_before);
    // update the appropriate pointers
    if (insert_before->prev) { insert_before->prev->next = elem; }
    insert_before->prev = elem;

    // if we just inserted at the head, update the list's head pointer
    if (index == 0)
    { list->head = elem; }

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

    // if 'fill_width' is enabled and the terminal width is too small, we'll
    // return NULL to indicate failure
    if (fill_width && get_terminal_width() < BOX_MIN_WIDTH)
    {
        fprintf(stderr, "Terminal size is too small.\n");
        return NULL;
    }

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
    int box_string_length = 0;
    for (int i = 0; i < list->size; i++)
    {
        int task_string_length = strlen(task_strings[i]);
        memmove(box_string + box_string_length, task_strings[i], task_string_length);
        box_string_length += task_string_length;

        if (i < list->size - 1)
        { memmove(box_string + box_string_length++, "\n", 1);}
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
    if (tasks_complete == 0) { percent_complete = 0.0; }
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


// ========================== File String Parsing ========================== //
char* task_list_get_scribe_string(TaskList* list)
{
    // check for a NULL pointer or a NULL 'name' pointer
    if (!list || !list->name) { return NULL; }

    // determine the length of the string we'll need
    int name_length = strlen(list->name);
    if (name_length > TASK_LIST_NAME_MAX_LENGTH)
    { name_length = TASK_LIST_NAME_MAX_LENGTH; }
    int length = name_length + 8;
    // allocate the string accordingly
    char* result = calloc(length + 1, sizeof(char));

    // copy the list name in, then the list size
    snprintf(result, name_length + 1, "%s", list->name);
    snprintf(result + name_length, length - name_length, ",%d", list->size);
    return result;
}

TaskList* task_list_new_from_scribe_string(char* string)
{
    // check for a NULL pointer
    if (!string) { return NULL; }

    // search for the first comma. Everything up to the comma represents the
    // name of the task list
    char* comma1 = strstr(string, ",");
    if (!comma1) { return NULL; }
    int name_length = comma1 - string;

    // adjust the name length as needed (to ensure it's below the maximum)
    if (name_length > TASK_LIST_NAME_MAX_LENGTH)
    { name_length = TASK_LIST_NAME_MAX_LENGTH; }

    // copy the name of the task list
    char name[name_length + 1];
    snprintf(name, name_length + 1, "%s", string);

    // create a new TaskList with the name
    TaskList* result = task_list_new(name);
    if (!result) { return NULL; }

    // return the task list
    return result;
}
