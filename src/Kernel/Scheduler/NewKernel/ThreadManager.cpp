//
// Created by root on 4/2/18.
//


#include "ThreadManager.h"

#include "core_arm_cortex_m4f.h"

#include <Kernel/Interaction/Interaction.h>


//First, let's determine the number of threads;

const uint8_t NB_THREADS = (uint8_t) 0

#define THREAD(i, ...) + ((uint8_t) 1)

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


/*
 * Private variables and function for ThreadManager Library;
 */

namespace ThreadManager {

    //Thread initializer;
    bool initialise_thread(uint8_t i, uint32_t size, void (*function)(void));

    //Threads main functions;
    void run(thread_data_t thread_data);

    //Threads termination functions;
    void termination_loop();

    //Threads contexts;
    thread_data_t threads_data[NB_THREADS];

    //The current task;
    uint8_t current_thread = 0;


#define THREAD(i, ...) void thread_function_##i() { run(threads_data[i]);}

   #include "threads_config.h"

#undef THREAD

}


void ThreadManager::addTask(task_data_t task) {

    for (uint8_t i = 0; i < NB_THREADS; i++) {

        if (threads_data[i].current_state == TERMINATED) {

            threads_data[i].current_state = PENDING;

            *threads_data[i].task = task;

        }

    }

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


/*
 * initialise_thread : this function initialises a single thread;
 */

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
    stack_ptr_t sp = (stack_ptr_t) stack_end + stack_size - 1;

    //Set the thread's stack pointer to the allocated memory zone;
    core_set_thread_stack_pointer(sp);

    //Initialise the stack;
    core_init_stack(function, termination_loop);

    //Get the new stack pointer;
    core_get_thread_stack_pointer(sp);

    //Save the new stack pointer;
    threads_data[thread_id].stack_pointer = sp;

}


/*
 * trigger_context_switch : enables the context switching at next thread exit;
 */

inline void trigger_context_switch() {

    //Call the core function;
    core_trigger_context_switch();

}


/*
 * run : this function is the base function of a thread;
 *
 *  It verifies that it can execute a task, then executes it if so.
 *
 *  Finally, it executes its termination function, and re-iterate;
 */

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

        }

        //Mark the thread inactive;
        thread_data.current_state = TERMINATED;

        //Require a context switch, as the task is finished;
        trigger_context_switch();

    }

}


/*
 * termination_loop : does nothing but to wait;
 */

void ThreadManager::termination_loop() {
    while (true);
}


/*
 * start : requires the user mode, and starts the execution of the first thread;
 */

void ThreadManager::start() {

    //Update the current thread;
    current_thread = 0;

    //Set the current stack pointer;
    core_set_thread_stack_pointer(threads_data[0].stack_pointer);

    //Require the user mode;
    core_set_user_mode();

    //Execute the first task;
    thread_function_0();

}


/*
 * set_current_task_sp : update the current task's stack pointer;
 */

void ThreadManager::set_current_task_sp(stack_ptr_t new_stack_pointer) {

    //Simply copy the new stack pointer in the appropriate thread data;
    threads_data[current_thread].stack_pointer = new_stack_pointer;

}


/*
 * schedule : selects a new thread to run, and return its stack pointer;
 */

stack_ptr_t ThreadManager::schedule() {


    //Basic round robin.
    if (!current_thread)
        current_thread = NB_THREADS;

    current_thread--;

    return threads_data[current_thread].stack_pointer;

}


/*
 * The context switcher;
 */

void PendSV_Handler() {

    //Stack the context in the current process stack;
    core_stack_thread_context();

    //Create a temp variable to contain the thread stack pointer;
    stack_ptr_t thread_stack_pointer = 0;

    //Get the stack pointer;
    core_get_thread_stack_pointer(thread_stack_pointer)

    //Save the current thread's stack pointer;
    ThreadManager::set_current_task_sp(thread_stack_pointer);

    //Get the next working thread's stack pointer;
    stack_ptr_t stack_pointer = ThreadManager::schedule();

    //Set the appropriate context;
    core_set_thread_stack_pointer(stack_pointer);

    //Unstack the context from the previous process stack
    core_unstack_thread_context();

}