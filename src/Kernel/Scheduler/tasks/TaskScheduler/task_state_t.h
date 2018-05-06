

#ifndef TRACER_TASK_STATE_T
#define  TRACER_TASK_STATE_T

enum task_state_t {
    complete,
    invalid_arguments,
    reprogram,
    error
};

typedef task_state_t (*task_function_t)(void*);

#endif