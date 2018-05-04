//
// Created by root on 4/2/18.
//

#ifndef TRACER_THREADMANAGER_H
#define TRACER_THREADMANAGER_H


#include "Kernel/Arch/Processors/core_arm_cortex_m4f.h"

#include "Kernel/Scheduler/TaskData.h"


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
 * The process structure;
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
     * process state;
     */

    process_state state;

} process;


//Allocate a stack for a process;
void process_create_context(process *process_p, uint32_t stack_size);

//Reset a process's context;
void process_reset_context(process *process);


//Set the working process's stack pointer;
void process_save_sp(stack_ptr_t new_stack_pointer);

//Lock a thread (semaphore call);
void process_lock(process *process);

//Unlock a thread (semaphore call);
void process_unlock(process *process);


#endif //TRACER_THREADMANAGER_H
