#ifndef TRACER_TASK_SCHEDULER_DATA_H
#define TRACER_TASK_SCHEDULER_DATA_H


#include "task_state_t.h"

typedef struct task_t {
    uint8_t type;
    task_state_t (*task)(void *);
    void * args;
};

#endif //TRACER_TASK_SCHEDULER_DATA_H
