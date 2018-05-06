//
// Created by root on 3/23/18.
//

#ifndef TRACER_TASKSEQUENCER_H
#define TRACER_TASKSEQUENCER_H


#include "stdint.h"

#include "stdbool.h"

#include <Kernel/Scheduler/tasks/task.h>


//--------------------- Initialisation ---------------------

//Reset all sequences;
void sequences_reset();


//--------------------- Tasks containers management ---------------------

//Get the number of task that can be inserted in a particular sequence;
uint8_t sequences_available_spaces(uint8_t type);

//Verify that tasks are available in the required sequences;
bool sequences_available_tasks(uint8_t type);

//Build and schedule a task;
bool sequences_add_task(uint8_t sequence_id, task_t *task);


//--------------------- Tasks execution --------------------

//Remove the current task of the given sequence;
void unlockSequence(uint8_t sequence_id);


//Determine the number of tasks in the task pool;
uint8_t poolTasksNb();

//Get a task from the task pool;
uint8_t getPoolTask();


//Determine if the required sequence can be processed;
bool poolProcessable(uint8_t type);

//Get the first task of the given sequence;
uint8_t getSequencerTask(uint8_t sequence_id);


#endif //TRACER_TASKSEQUENCER_H
