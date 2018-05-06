//
// Created by root on 4/9/18.
//

#ifndef TRACER_TASKDATA_H
#define TRACER_TASKDATA_H

#include "stdint.h"


typedef struct {
    //TODO INTERGRATE THE PRIORITY DATA;
} prio_data_t;


/*
 * The type of task;
 */
typedef enum {

    //The task is persistent (event or synchronous) Must not be deleted after completion;
            PERSISTENT_TASK,

    //The task comes from a sequence; Must be deleted after completion;
            SEQUENCE_TASK,

} task_type_t;


/*
 * If the task is persistent, we must know its status, to be sure that we don't delete a running task;
 */

typedef enum {

    //The task is not started, and is stored in its container;
            STORED,

    //The task is started, but is still stored in its container. Mustn't be deleted;
            RUNNING,

    //The task is out of its container, must be deleted when it reaches task_cleanup;
            DELETABLE

} persistent_task_state_t;


/*
 * The task structure;
 */

typedef struct {

    //The task's function;
    void (*function)(void *args);

    //The function's arguments;
    void *args;

    //The cleanup function to execute after the task's function;
    void (*cleanup)();

    //The origin of the task
    const task_type_t task_type;

    //An union, that specifies information depending on the task's origin;
    union {

        //If the task comes from a sequence, its id;
        uint8_t sequence_id;

        //If the task comes from an event, or is synchronous, its state;
        persistent_task_state_t persistent_task_state;

    };

} task_t;


//---------------------- Public Functions ----------------------

//Add a task;
task_t *task_create(void (*func)(void *), void *args, void (*cleanup)(), task_type_t origin);

//Eventually delete a task;
void task_cleanup(task_t *);

#endif //TRACER_TASKDATA_H
