#ifndef TRACER_TASK_SCHEDULER_DATA_H
#define TRACER_TASK_SCHEDULER_DATA_H


typedef struct task_t {
    uint8_t type;
    bool (*task)(void *);
    void * args;
};

#endif //TRACER_TASK_SCHEDULER_DATA_H
