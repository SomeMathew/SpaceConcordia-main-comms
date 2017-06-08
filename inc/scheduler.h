/**
 * @file scheduler.h
 * @author Space Concordia Rocket division
 * @author Mathieu Breault
 * @brief Basic task scheduler for event based system, without pre-emption and minimal support
 * for priorities.
 * 
 * This module can be used to handle recurring or non-recurring tasks or events with priorities.
 * The scheduler must always have 1 active task to work correctly, to prevent undefined behaviour,
 * always have a repeated task active.
 * The work loop of the program must call runScheduler often to update the scheduler.
 * 
 * How to use:
 * 	-Add a task with createTask() with a function pointer which will be called when the task/event
 * 	  is ready to run. The timeInterval is set according to the sysTimer.h implementation. The priority
 * 	  is between 0 and (TASKS_PRIORITY_COUNT - 1).
 *  -Periodically update the scheduler with runScheduler(), it will update the task/event queue and
 *    call any ready tasks.
 *  -A task can destroy itself or an other task by using destroyTask(). This will fail if it is
 *    the last remaining tasks.
 * 
 * Dependency:
 * 	sysTimer.h must be implemented to give a time interval.
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>
#include <stdbool.h>


#define TASKS_MAX_COUNT 24 // Limited to 256
#define TASKS_PRIORITY_COUNT 3 // temporary

struct task;

bool runScheduler(void);
struct task * createTask(void (*vector)(uint32_t, void *), uint32_t event, void * argument,
		uint32_t timeInterval, bool repeat, uint8_t priority);
bool destroyTask(struct task *);

#endif /* SCHEDULER_H_ */
