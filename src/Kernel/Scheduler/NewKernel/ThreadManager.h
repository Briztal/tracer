//
// Created by root on 4/2/18.
//

#ifndef TRACER_THREADMANAGER_H
#define TRACER_THREADMANAGER_H


#include "core_arm_cortex_m4f.h"

#include "TaskData.h"



/*
 * The thread manager manages threads.
 *
 *  A Thread is a context of execution, in which we can execute any function we want.
 *
 *  Threads are initialised at boot, and are fed with functions during the execution;
 *
 */

namespace ThreadManager {

    //TODO SCHEDULER SPECIFIC
    //Task add;
    void addTask(TaskData &task);

    //Select a new task and return its stack pointer;
    stack_ptr_t schedule();


    //---------------------------- Public functions ----------------------------

    //Thread creator;
    bool createThread(uint32_t stack_size);

    //Start threads execution;
    void start();

    //Set the current task's stack pointer;
    void set_current_task_sp(stack_ptr_t new_stack_pointer);

};


#endif //TRACER_THREADMANAGER_H
