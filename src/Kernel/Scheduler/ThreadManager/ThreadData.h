//
// Created by root on 4/10/18.
//

#ifndef TRACER_THREADDATA_H
#define TRACER_THREADDATA_H


#include "Kernel/Scheduler/TaskData.h"
#include "Kernel/Scheduler/core_arm_cortex_m4f.h"

#include <stdint.h>

//The set of execution states;
enum thread_state {

    //Execution required;
            PENDING,

    //Execution in progress;
            ACTIVE,

    //Execution done;
            TERMINATED

};


struct ThreadData {

    //The current stack pointer;
    stack_ptr_t stack_pointer = nullptr;

    //The beginning of the stack;
    stack_ptr_t stack_begin = nullptr;

    //The end of the stack;
    stack_ptr_t stack_end = nullptr;

    //The current execution state;
    thread_state current_state = TERMINATED;

    //The current task in execution;
    uint8_t task_index = 0;

};


#endif //TRACER_THREADDATA_H
