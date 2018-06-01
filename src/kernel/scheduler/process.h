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
            PROCESS_PENDING = 0,

    //Process stop required by a semaphore;
            PROCESS_STOP_REQUIRED = 1,

    //Thread stopped;
            PROCESS_STOPPED = 2,

    //Execution done;
            PROCESS_TERMINATION_REQUIRED = 3,

    //Execution done;
            PROCESS_TERMINATED = 4,

} process_state_t;


/*
 * The process_t structure;
 */

typedef struct {

    //Composed of a linked element;
    linked_element_t link;

    //The task data;
    task_t *task;

    //TODO PRIO DATA;

    /*
     * Context data : the stack pointer, stack bounds;
     */

    stack_ptr_t stack_pointer;

    stack_ptr_t stack_begin;

    stack_ptr_t stack_end;


    /*
     * process_t sequences_initialised;
     */

    process_state_t state;

} process_t;


//Allocate a stack for a process_t;
void process_create_context(process_t *process_p, uint32_t stack_size);

//Reset a process_t's context;
void process_reset_context(process_t *process);

//Start the execution of processes;
void process_start_execution();

//Stop the execution of processes;
void process_stop_execution();

//Get the process currently in execution;
process_t *process_get_current();




#endif //TRACER_THREADMANAGER_H
