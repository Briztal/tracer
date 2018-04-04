#ifndef TRACER_TASK_SCHEDULER_DATA_H
#define TRACER_TASK_SCHEDULER_DATA_H


#include "task_function_t.h"
#include "task_state_t.h"
#include <Kernel/Interaction/CommunicationPipe.h>


struct task_t {

    /*
     * The function to execute, at runtime.
     */

    task_function_t task = nullptr;


    /*
     * Arguments that must be passed to the function.
     *  As the signature of a task's function is fixed, we can only pass a pointer to a structure containing the
     *      arguments
     *
     * This structure MUST BE DECLARED IN THE HEAP, as it will be freed automatically, to avoid memory leak.
     */

    void *dynamic_args = nullptr;


    /*
     * The communication pipe :
     *
     *  If log is required (logs), this pipe will handle it;
     */

    CommunicationPipe *log_pipe;

};

#endif //TRACER_TASK_SCHEDULER_DATA_H
