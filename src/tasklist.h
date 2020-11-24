// A module that defines struct(s) and functions that represent a list of
// tasks. The TaskList is a doubly-linked, dynamically-allocated list.
//
//      Connor Shugg

// Module inclusions
#include "task.h"

// =========================== List Elem Struct ============================ //
// The 'TaskListElemn' struct represents a single node of a TaskList.
typedef struct _TaskListElem
{
    Task* task;
    TaskListElem* prev;
    TaskListElem* next;
} TaskListElem;

// ============================== List Struct ============================== //
// The 'TaskList' struct represents a list of Tasks.
typedef struct _TaskLisk
{
    char* name;             // name of the task list
    int size;               // number of tasks in the list
    TaskListElem* head;     // head node of the linked list
} TaskList;

// Constructor: dynamically allocates a new TaskList pointer. If allocation
// fails, NULL is returned.
TaskList* task_list_new(char* list_name);

// Destructor: frees a task list an all of its inner TaskListElems (including
// each TaskListElem's inner Task pointer).
void task_list_free();

// Takes a dynamically-allocated Task pointer and attempts to insert it into
// the list. On success, 0 is returned. A non-zero value is returned otherwise.
int task_list_insert(Task* task);

// Searches the task list for a task with the given name. If a task is found,
// the pointer to the Task struct is returned. Otherwise, NULL is returned.
Task* task_list_search(char* task_name);

// Takes in a pointer to a Task struct and searches the list for the
// TaskListElem that contains the pointer. If it's found, the item is removed
// and the Task pointer is returned. It's up to the caller to free the Task
// pointer's memory.
int task_list_remove(Task* task);
