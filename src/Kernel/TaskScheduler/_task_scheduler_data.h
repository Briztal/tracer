#ifndef TRACER_TASK_SCHEDULER_DATA_H
#define TRACER_TASK_SCHEDULER_DATA_H


#include "task_function_t.h"
#include "task_state_t.h"

struct task_t {

    /*
     * The type of the task :
     *  - 255 for a task to execute as soon as possible
     *  - other values for task to parse in order.
     *      WARNING : The order only applies for task belonging to the same group!
     */

    uint8_t type = 255;


    /*
     * The function to parse, at runtime.
     */

    task_function_t task = nullptr;


    /*
     * Arguments that must be passed to the function.
     *  As the signature of a task's function is fixed, we can only pass a pointer to a structure containing the
     *      arguments
     *
     * This structure MUST BE DECLARED IN THE HEAP, as it will be freed automatically, to avoid memory leak.
     *
     */

    void *dynamic_args = nullptr;


    /*
     * The communication method :
     *  If communication is required (logs), this function will take care of the encoding, and transmit
     *      the encoded string, using a particular log_protocol.
     *
     *  The log_protocol, as the message, must be provided.
     */

    //void (*log_function)(Delimiter *, const char *message) = nullptr;


    /*
     * The log_protocol used to communicate.
     */

    //Delimiter *log_protocol = nullptr;


};

#endif //TRACER_TASK_SCHEDULER_DATA_H
