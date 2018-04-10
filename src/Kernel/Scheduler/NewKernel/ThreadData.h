//
// Created by root on 4/10/18.
//

#ifndef TRACER_THREADDATA_H
#define TRACER_THREADDATA_H


#include "TaskData.h"
#include "core_arm_cortex_m4f.h"

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


class ThreadData {

public:

    //Constructor;
    ThreadData() :
            task(nullptr),
            current_state(TERMINATED),
            stack_pointer(nullptr),
            stack_begin(nullptr),
            stack_end(nullptr) {};

    //Copy constructor : disabled;
    ThreadData(uint32_t stack_size);

    //Move constructor : disabled;
    ThreadData(ThreadData &&src) noexcept : ThreadData(){};


public:

    //The current task in execution;
    TaskData *task;

    //The current execution state;
    thread_state current_state = TERMINATED;

    //The current stack pointer;
    stack_ptr_t stack_pointer;

    //The beginning of the stack;
    stack_ptr_t stack_begin;

    //The end of the stack;
    stack_ptr_t stack_end;

};


#endif //TRACER_THREADDATA_H
