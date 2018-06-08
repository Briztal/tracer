//
// Created by root on 4/2/18.
//

#ifndef TRACER_THREADMANAGER_H
#define TRACER_THREADMANAGER_H


#include "kernel/arch/arch.h"

#include "kernel/scheduler/tasks/task.h"


//The set of execution states;
typedef enum {

    //Execution required;
            SPROCESS_PENDING = 0,

    //Process stop required by a semaphore;
            SPROCESS_STOP_REQUIRED = 1,

    //Thread stopped;
            SPROCESS_STOPPED = 2,

    //Execution done;
            SPROCESS_TERMINATION_REQUIRED = 3,

    //Execution done;
            SPROCESS_TERMINATED = 4,

} sprocess_state_t;


/*
 * The process_t structure;
 */

typedef struct {

    //Composed of a linked element;
    linked_element_t link;

    //The process stack;
	stack_t stack;

    //The process state;
   	sprocess_state_t state;

    //The task data;
    task_t *task;

    //TODO PRIO DATA;

} sprocess_t;


//Create a sprocess in the heap with a stack of the required size;
sprocess_t *sprocess_create(size_t stack_size);

//Reset a process;
void sprocess_reset(sprocess_t *sprocess);

//Delete a process, and free its stack;
void sprocess_delete(sprocess_t *sprocess);

//Execute a process. If succeeds, doesn't return;
void sprocess_execute(sprocess_t *sprocess);


#endif //TRACER_THREADMANAGER_H
