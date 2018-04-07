//
// Created by root on 4/2/18.
//

#ifndef TRACER_THREADMANAGER_H
#define TRACER_THREADMANAGER_H

#include "Arduino.h"

#include "core_arm_cortex_m4f.h"

typedef struct {

    //The task's function;
    void (*function)(void *args);

    //The function's arguments;
    void *args;

    //The termination function to execute after the task's function;
    void (*termination)();

} task_data_t;

/*
 * The thread manager manages threads.
 *
 *  A Thread is a context of execution, in which we can execute any function we want.
 *
 *  Threads are initialised at boot, and are fed with functions during the execution;
 *
 */

namespace ThreadManager {

    //Threads initializer;
    bool initialise_threads();

    //TODO TASK MANAGER SPECIFIC
    //Task add;
    void addTask(task_data_t task);

    //Start threads execution;
    void start();

    //Set the current task's stack pointer;
    void set_current_task_sp(stack_ptr_t new_stack_pointer);

    //Select a new task and return its stack pointer;
    stack_ptr_t schedule();

};


#endif //TRACER_THREADMANAGER_H
