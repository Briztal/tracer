//
// Created by root on 4/2/18.
//


#include "ThreadManager.h"

#include <Kernel/Interaction/Interaction.h>


//First, let's determine the number of threads;

const uint8_t NB_THREADS = (uint8_t) 0

#define THREAD(i, ...) (+ (uint8_t 1)

#include "threads_config.h"

;

#undef THREAD


/*
 * The task data structure;
 *
 *  A task will contain :
 *
 *      - a function to execute;
 *
 *      - a pointer to arguments (that will be casted by the function at execution time);
 *
 *      - a function to execute at the end;
 *
 */

typedef struct {

    //The task's function;
    void (*function)(void *args);

    //The function's arguments;
    void *args;

    //The termination function to execute after the task's function;
    void (*termination)();

} task_data_t;



//The set of execution states;
enum thread_state {

    //Execution required;
            PENDING,

    //Execution in progress;
            ACTIVE,

    //Execution done;
            TERMINATED

};


/*
 * The thread data structure. Contains the current task in execution and the current state of execution;
 */

typedef struct {

    //The current task in execution;
    task_data_t *task = nullptr;

    //The current execution state;
    thread_state current_state = TERMINATED;

    //The maximum value of the thread's stack;
    uint32_t *stack_pointer;

} thread_data_t;


//TODO KERNEL SPECIFIC;
//The stack pointer type;
typedef uint32_t *stack_ptr_t;



/*
 * Private variables and function for ThreadManager Library;
 */

namespace ThreadManager {

    //Threads initializer;
    bool initialise_threads();

    //Thread initializer;
    bool initialise_thread(uint8_t i, uint32_t size, void (*function)(void));

    //Threads main functions;
    void run(thread_data_t thread_data);

    //Threads contexts;
    thread_data_t threads_data[NB_THREADS];

    //The current task;
    uint8_t current_task;


#define THREAD(i, ...) void thread_function_##i() { run(threads_data[i]);}

#include "threads_config.h"

#undef THREAD

}



/*
 * initialise_threads : this function will initialise all threads;
 */

bool ThreadManager::initialise_threads() {


    //Initialise all threads and return true if all happened correctly;
    return true

        //Add every initialisation;
#define THREAD(i, size) && initialise_thread(i, size, thread_function_##i)

#include "threads_config.h"

        //End the expression;
;

#undef THREAD


}

bool ThreadManager::initialise_thread(uint8_t thread_id, uint32_t stack_size, void (*function)(void)) {

    //First, free the eventually allocated memory;
    free(threads_data[thread_id].stack_pointer);

    //Then, allocate a new stack of the required size;
    void *stack_end = malloc(stack_size);

    //If the allocation failed :
    if (stack_end == nullptr) {

        //Log;
        std_out("Error in ThreadManager::initialise_threads : the stack allocation failed;");

        //Fail;
        return false;

    }

    //As stack is decreasing, we must set the stack pointer as the array's end;
    stack_ptr_t sp = threads_data[thread_id].stack_pointer = (stack_ptr_t) stack_end + stack_size - 1;

    //Set the thread's stack pointer to the allocated memory zone;
    kernel_set_thread_stack_pointer(sp);

    //Stack the current context. This way, at runtime, context unstack won't dequeue to unallocated space;
    kernel_stack_thread_context();

    //Get the address of the function;
    void *function_pointer = (void*)function;

    //TODO SET THE PROGRAM COUNTER OF THE PREVIOUS CONTEXT;
    kernel_set_stacked_pc(function_pointer);

}

inline void trigger_context_switch() {

    NVIC_SET_PENDING(-2);

}


void ThreadManager::run(thread_data_t thread_data) {

    //Infinite loop;
    while (true) {

        //If the task must be executed :
        if (thread_data.current_state == PENDING) {

            //Mark the thread Active;
            thread_data.current_state = ACTIVE;

            //Cache the task;
            task_data_t *task_p = thread_data.task;

            //Execute the task;
            task_p->function(task_p->args);

            //Terminate the task;
            task_p->termination();

            //Mark the task finished;
            thread_data.current_state = TERMINATED;

        }

        //Require a context switch, as the task is finished;
        trigger_context_switch();

    }

}


/*
 * The context switcher;
 */

void PendSV_Handler() {

    //Save the current thread's stack pointer;
    ThreadManager::set_current_task_sp(kernel_get_thread_stack_pointer());

    //Stack the context in the current process stack;
    kernel_stack_thread_context();

    //Get the next working thread's stack pointer;
    uint32_t stack_pointer = ThreadManager::switch_task();

    //Set the appropriate context;
    kernel_set_thread_stack_pointer(stack_pointer);

    //Unstack the context from the previous process stack
    kernel_unstack_thread_context();

}