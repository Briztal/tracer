//
// Created by root on 4/9/18.
//

#ifndef TRACER_TASKDATA_H
#define TRACER_TASKDATA_H

#include "stdint.h"

#include <data_structures/containers/llist.h>


typedef struct {
    //TODO INTERGRATE THE PRIORITY DATA;
} prio_data_t;


/*
 * The type of task;
 */
typedef enum {

    //The task is the loop executed by the scheduler when in inactive state;
            EMPTY_TASK,

    //The task is persistent (event or synchronous) Must not be deleted after completion;
            SERVICE_TASK,

    //The task comes from a sequence; Must be deleted after completion;
            SEQUENCE_TASK,

} task_type_t;


/*
 * If the task is persistent, we must know its status, to be sure that we don't delete a running task;
 */
/*
typedef enum {

    //The task is not started, and is stored in its container;
            STORED_TASK,

    //The task is started, but is still stored in its container. Mustn't be deleted;
            RUNNING_TASK,

    //The task is out of its container, must be deleted when it reaches task_cleanup;
            CLOSED_TASK,

} persistent_task_state_t;

*/
/*
 * The task structure;
 *
 *  Tasks are are not executed and removed in a particular order. They must be stored as linked elements.
 *
 *  A task is composed of a linked element, and of all data related to a task;
 */

typedef struct {

    //The linked element part;
    linked_element_t linked_element;

    //The task's function;
    void (*const function)(void *args);

    //The function's arguments;
    const void *const args;

    //The cleanup function to execute after the task's function;
    void (*const cleanup)();

    //The origin of the task
    const task_type_t task_type;

} task_t;


//---------------------- Public Functions ----------------------

//Delete entirely a heap stored task composed struct;
void task_delete(task_t *task_p);

#endif //TRACER_TASKDATA_H
