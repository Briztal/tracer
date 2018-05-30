//
// Created by root on 3/23/18.
//

#ifndef TRACER_TASKSEQUENCER_H
#define TRACER_TASKSEQUENCER_H


#include "stdint.h"

#include "stdbool.h"

#include <kernel/scheduler/tasks/task.h>

/*
 * The sequence task type is composed of a task and of a sequence identifier;
 */

typedef struct {

    //Task composition;
    task_t task;

    //Sequence identifier;
    uint8_t sequence_id;

} sequence_task_t;


//--------------------- Initialisation ---------------------

//Set the pool size;
void sequences_init(size_t pool_size);

//Add a sequence of the given size;
void sequences_add_sequence(size_t sequence_size);


//--------------------- Tasks containers management ---------------------

//Get the number of task that can be inserted in a particular sequence;
bool sequence_insertion_available(uint8_t sequence_id);

//Verify that tasks are available in the required sequences;
bool sequences_available_task(uint8_t type);

//Build and schedule a task;
bool sequences_add_task(uint8_t sequence_id, void (*func)(void *), void *args, void (*cleanup)());

//--------------------- Tasks execution --------------------

//Remove the current task of the given sequence;
void sequences_remove_task(sequence_task_t *task);

//Get the first task of the given sequence;
task_t *sequences_get_task(uint8_t sequence_id);


#endif //TRACER_TASKSEQUENCER_H