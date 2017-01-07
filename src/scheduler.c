/**
 * @file scheduler.c
 * @author Space Concordia Rocket division
 * @author Mathieu Breault
 * @brief Basic task scheduler for event based system, without pre-emption.
 * 
 * The scheduler is implemented using 2 ordered lists, an array of list readyTasksLists and a 
 * waitTasksList. The readyTasksLists are in buckets according to their priority. 
 * 
 * When runScheduler() is called, it will sequentially checks the readyTasks by priorities and call the
 * next task's vector that is ready. It will then move any ready tasks from the waitTasksList to the
 * readyTasksLists.
 * The user must periodically call runScheduler() in the work loop.
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>

#include "scheduler.h"
#include "sysTimer.h"
#include "linkedList.h"

enum taskStatus {
    TASK_READY, TASK_WAIT,
};

struct task {
    struct linkedList_node link;
    struct task * nextEmpty;
    void (*vector)(uint32_t, void *);
    //~ void (*vector)(void *);
    uint32_t event;
    void * argument;
    uint8_t priority;
    uint32_t timeInterval;
    uint32_t timeLastEnd;
    bool repeat;
    enum taskStatus status;
};

static size_t tasksCount = 0;
static struct task taskNodes[TASKS_MAX_COUNT];
static struct task * nextEmptyTask = taskNodes;
static struct linkedList readyTasksLists[TASKS_PRIORITY_COUNT];
static struct linkedList waitTasksList;

static bool initTasksLists(void);
static bool runNextReadyTask(void);
static bool prepareNewReadyTasks(void);
static int compareWaitTasks(void * task1, void * task2);
static bool toggleTaskWait(struct task * task);
static bool toggleTaskReady(struct task * task);

bool runScheduler(void) {
    if (tasksCount == 0) {
        return false;
    }

    bool doExitScheduler = false;
    while (!doExitScheduler) {
        runNextReadyTask();
        prepareNewReadyTasks();
    }

    return true;
}

static inline bool timeIsAfter(uint32_t a, uint32_t b) {
    return ((int32_t) (b - a) < 0);

}

static inline bool timeIsBefore(uint32_t a, uint32_t b) {
//    return ((a - b) < (UINT32_MAX / 2));
    return timeIsAfter(b, a);
}

// Returns NULL on error
struct task * createTask(void (*vector)(uint32_t, void *), uint32_t event, void * argument,
		uint32_t timeInterval, bool repeat, uint8_t priority) {
    // TODO verify input values
    if (tasksCount >= TASKS_MAX_COUNT || vector == NULL) {
        return NULL;
    }

    // init Lists if this is the first task, after that there will always be a min of 1
    if (tasksCount == 0) {
        initTasksLists();
    }

    // fetch from and set free list
    struct task * newTask = nextEmptyTask;
    nextEmptyTask = nextEmptyTask->nextEmpty;

    newTask->nextEmpty = NULL;
    newTask->vector = vector;
    newTask->event = event;
    newTask->argument = argument;
    newTask->priority = priority;
    newTask->timeInterval = timeInterval;
    newTask->timeLastEnd = 0;
    newTask->repeat = repeat;
    newTask->status = TASK_READY;
    tasksCount++;

    // task must always be in exactly one status list
    // init as ready to run
    linkedList_initNode((struct linkedList_node *) newTask, newTask);
    struct linkedList * readyList = &readyTasksLists[newTask->priority];
    linkedList_addBefore(readyList, (struct linkedList_node *) newTask, &readyList->head);

    return newTask;
}

bool destroyTask(struct task * task) {
    // there can't be 0 task
    if (tasksCount <= 0) {
        return false;
    }

    linkedList_remove((struct linkedList_node *) task);
    task->vector = NULL;
    task->argument = NULL;

    // set the free list
    task->nextEmpty = nextEmptyTask;
    nextEmptyTask = task;

    tasksCount--;
    return true;
}

static bool runNextReadyTask(void) {
    // priority High to Low
    for (int priority = 0; priority < TASKS_PRIORITY_COUNT; priority++) {
        if (readyTasksLists[priority].length > 0) {
            struct task * nextTask = readyTasksLists[priority].head.next->element;
            nextTask->vector(nextTask->event, nextTask->argument);
            nextTask->timeLastEnd = sysTimer_GetTick();
            // skip toggle/destroy if the task destroyed itself
            if (nextTask->vector != NULL) {
                if (nextTask->repeat) {
                    toggleTaskWait(nextTask);
                } else {
                    destroyTask(nextTask);
                }
            }

            break;
        }
    }
    return true;
}

static bool prepareNewReadyTasks(void) {
    uint32_t currentTime = sysTimer_GetTick();

    if (waitTasksList.length <= 0) {
        return true;
    }
    struct task * taskCursor = (struct task *) waitTasksList.head.next;
    while (taskCursor != (struct task *) &waitTasksList.head) {
        uint32_t nextTaskRunTime = taskCursor->timeLastEnd + taskCursor->timeInterval;
        if (timeIsBefore(nextTaskRunTime, currentTime) || (nextTaskRunTime == currentTime)) {
            // Update cursor before setting task to ready, since the task's list will change
            taskCursor = (struct task *) taskCursor->link.next;
            toggleTaskReady((struct task *) taskCursor->link.previous);
        } else {
            break;
        }
    }
    return true;
}

static bool toggleTaskWait(struct task * task) {
    task->status = TASK_WAIT;
    linkedList_remove((struct linkedList_node *) task);
    linkedList_addOrdered(&waitTasksList, (struct linkedList_node *) task, compareWaitTasks);
    return true;
}

static bool toggleTaskReady(struct task * task) {
    task->status = TASK_READY;
    linkedList_remove((struct linkedList_node *) task);
    struct linkedList * readyList = &readyTasksLists[task->priority];
    linkedList_addBefore(readyList, (struct linkedList_node *) task, &readyList->head);
    return true;
}

static int compareWaitTasks(void * task1, void * task2) {
    struct task * t1 = task1;
    struct task * t2 = task2;
    uint32_t t1TimeNextRun = t1->timeLastEnd + t1->timeInterval;
    uint32_t t2TimeNextRun = t2->timeLastEnd + t2->timeInterval;

    if (timeIsBefore(t1TimeNextRun, t2TimeNextRun)) {
        return -1;
    } else if (timeIsAfter(t1TimeNextRun, t2TimeNextRun)) {
        return 1;
    } else {
        return 0;
    }

}

static bool initTasksLists(void) {
    // last node must keep a NULL nextEmpty
    for (int i = 0; i < (TASKS_MAX_COUNT - 1); i++) {
        taskNodes[i].nextEmpty = &taskNodes[i + 1];
    }

    for (int i = 0; i < TASKS_PRIORITY_COUNT; i++) {
        linkedList_initList(&readyTasksLists[i]);
    }
    linkedList_initList(&waitTasksList);

    return true;
}
