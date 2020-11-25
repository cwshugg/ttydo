// A module that defines struct(s) and functions that represent a list of
// tasks. The TaskList is a doubly-linked, dynamically-allocated list.
//
//      Connor Shugg

// Module inclusions
#include "task.h"

// ========================== Constants & Macros =========================== //
#define TASK_LIST_NAME_MAX_LENGTH 64    // maximum character count for a name

// =========================== List Elem Struct ============================ //
// The 'TaskListElemn' struct represents a single node of a TaskList.
typedef struct _TaskListElem
{
    Task* task;
    struct _TaskListElem* prev;
    struct _TaskListElem* next;
} TaskListElem;

// Takes in a Task pointer and two TaskListElem pointers and uses them to make
// a new dynamically-allocated TaskListElem. The pointer is returned. If
// allocation fails, NULL is returned.
TaskListElem* task_list_elem_new(Task* payload, TaskListElem* previous, TaskListElem* next);

// Takes in a TaskListElem pointer and attempts to free its memory. Any other
// TaskListElems pointed at the by this elem's 'prev' and 'next' fields are
// ignored. The inner Task* payload is not freed - instead, it's returned.
Task* task_list_elem_free(TaskListElem* elem);


// ============================== List Struct ============================== //
// The 'TaskList' struct represents a list of Tasks.
typedef struct _TaskLisk
{
    char* name;             // name of the task list
    int size;               // number of tasks in the list
    TaskListElem* head;     // head node of the linked list
    TaskListElem* tail;     // the tail node of the linked list
} TaskList;

// Constructor: dynamically allocates a new TaskList pointer. If allocation
// fails, NULL is returned.
TaskList* task_list_new(char* list_name);

// Destructor: frees a task list an all of its inner TaskListElems (including
// each TaskListElem's inner Task pointer).
void task_list_free(TaskList* list);

// Takes a dynamically-allocated Task pointer and attempts to add it to the end
// of the list. On success, 0 is returned. A non-zero value is returned
// otherwise.
int task_list_append(TaskList* list, Task* task);

// Uses the given index to get the Task stored at the corresponding node (as if
// the linked list was an array). The Task* is returned on success, and NULL
// is returned on failure.
Task* task_list_get_by_index(TaskList* list, int list_index);

// Searches the task list for a task with the given name. If a task is found,
// the pointer to the Task struct is returned. Otherwise, NULL is returned.
Task* task_list_get_by_title(TaskList* list, char* task_title);

// Takes in a pointer to a Task struct and searches the list for the
// TaskListElem that contains the pointer. If it's found, the item is removed
// and the Task pointer is returned. It's up to the caller to free the Task
// pointer's memory. (If the task isn't found and removed, NULL is returned.)
Task* task_list_remove(TaskList* list, Task* task);
