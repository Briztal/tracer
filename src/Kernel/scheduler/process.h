//
// Created by root on 4/2/18.
//

#ifndef TRACER_THREADMANAGER_H
#define TRACER_THREADMANAGER_H


#include "Kernel/arch/arch.h"

#include "Kernel/scheduler/tasks/task.h"


//The set of execution states;
typedef enum {

    //Execution required;
            PENDING = 0,

    //Execution in progress;
            ACTIVE = 1,

    //Thread stopped by a semaphore;
            STOPPED = 2,

    //Execution done;
            TERMINATED = 3

} process_state;


/*
 * The process_t structure;
 */

typedef struct {

    /*
     * The priority determination structure;
     */

    //TODO PRIO DATA;

    /*
     * The execution data : the main function, its args, and the exit function;
     */

    void (*process_function)(void *);

    void *args;

    void (*exit_function)();


    /*
     * Context data : the stack pointer, stack bounds;
     */

    stack_ptr_t stack_pointer;

    stack_ptr_t stack_begin;

    stack_ptr_t stack_end;


    /*
     * process_t state;
     */

    process_state state;

} process_t;


//Allocate a stack for a process_t;
void process_create_context(process_t *process_p, uint32_t stack_size);

//Reset a process_t's context;
void process_reset_context(process_t *process);

//Start the execution of processes;
void process_start_execution();

//Get the process currently in execution;
process_t *process_get_current();

//Set the working process_t's stack pointer;
void process_save_sp(stack_ptr_t new_stack_pointer);

//Lock a thread (semaphore call);
void process_lock(process_t *process);

//Unlock a thread (semaphore call);
void process_unlock(process_t *process);


#endif //TRACER_THREADMANAGER_H
