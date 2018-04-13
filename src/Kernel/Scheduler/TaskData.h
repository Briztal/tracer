//
// Created by root on 4/9/18.
//

#ifndef TRACER_TASKDATA_H
#define TRACER_TASKDATA_H

#include "stdint.h"

#include <Kernel/Scheduler/TaskScheduler/task_state_t.h>

enum task_origin_t {

    //The most general (and default) type;
    INTERNAL_TASK,

    //The task is synchronous;
    SYNCHRONOUS_TASK,

    //The task comes from an event;
    EVENT_TASK,

    //The task comes from a sequence;
    SEQUENCE_TASK,

};

struct TaskData {

    //The task's function;
    task_state_t (*function)(void *args) = nullptr;

    //The function's arguments;
    void *args = nullptr;

    //The termination function to execute after the task's function;
    void (*termination)() = nullptr;

    //The identifier of the task;
    uint32_t task_id = 0;

    //The origin of the task
    task_origin_t task_origin = INTERNAL_TASK;

    //A variable fields, containing information depending on the origin of the task
    uint8_t task_sub_type = 0;

};


#endif //TRACER_TASKDATA_H
