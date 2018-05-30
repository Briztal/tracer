//
// Created by root on 4/2/18.
//


#include <malloc.h>
#include <kernel/scheduler/tasks/task.h>
#include <kernel/kernel.h>
#include "stdint.h"

#include "stdbool.h"


#include "process.h"
#include "systick.h"
#include "scheduler.h"


//---------------------------- Private functions ----------------------------


//The function to start a thread on the fly;
void process_init();


//Threads main functions;
void process_function(volatile process_t *volatile process);

//Threads cleanup functions : infinite loop;
void process_exit() { while (1); }

//The process_t context switcher;
void process_preempt();

//The process currently in execution;
process_t *current_process;



//---------------------------- Process initialisation ----------------------------

/*
 * initialise_thread : this function allocates a stack, and then initialises stack pointers;
 *
 *  Finally, it sets the thread's sequences_initialised as Terminated;
 */

void process_create_context(process_t *process, uint32_t stack_size) {

    //Then, allocate a memory zone of the required size;
    void *ptr = malloc(stack_size * sizeof(stack_element_t));

    //If the reallocation failed :
    if (ptr == 0) {
        return;//TODO EXCEPTION;
    }

    //Initialise the thread's stack data;
    stack_ptr_t sp = (stack_ptr_t) ptr;

    //Set the stack's begin;
    process->stack_pointer = process->stack_begin = core_get_stack_begin(sp, stack_size);

    //Set the stack's end;
    process->stack_end = core_get_stack_end(sp, stack_size);

    //Reset the process_t context;
    process_reset_context(process);

}


/*
 * process_reset_context : resets the process_t to its initial stack pointer,
 * and initialises the context to its beginning;
 */

void process_reset_context(process_t *process) {

    //Declare a stack pointer;
    stack_ptr_t sp = process->stack_begin;

    //Set the thread's stack pointer to the allocated memory zone;
    core_set_thread_stack_pointer(sp);

    //Initialise the stack;
    core_init_stack(process_init, process_exit, (stack_element_t) process);

    //Save the new stack pointer;
    process->stack_pointer = core_get_thread_stack_pointer();

    //As the process_t is just reset, he potentially have no function to execute. It is so terminated by default;
    process->state = PROCESS_TERMINATED;

}


/*
 * process_init : this function is called when a process_t is started on the fly, without proper function call, due to
 *  a context switch;
 *
 * TODO CHANGE THIS SHIT ! INITIALISE ALL THREADS AND EXECUTE THREAD 0 WITH A LONGJMP
 *
 *  As the process_t pointer has been saved in the register at the thread initialisation, the first step is to get it back;
 *
 *  Then, we can call properly the run function;
 */

void process_init() {

    //Get the thread index and run the associated thread;
    process_function((volatile process_t *volatile) core_get_process());

}


/*
 * process_function : this function is the routine executed by a process_t;
 */

void process_function(volatile process_t *volatile process) {

    //The process_t must be pending, otherwise, there has been an error in the scheduler.
    if ((volatile process_state_t) process->state != PROCESS_PENDING) {

        //Kernel panic;
        //TODO KERNEL PANIC;

    }

    //Cache the task pointer;
    volatile task_t *volatile task = process->task;

    //Execute the function, passing args;
    (*(task->function))((void *) task->args);

    //Cache the exit function;
    void (*volatile cleanup)() = task->cleanup;

    //If the function is not null, execute it;
    if (cleanup) {
        (*cleanup)();
    }

    //Mark the process_t inactive; Scheduler will be notified at context switch;
    process->state = (volatile process_state_t) PROCESS_TERMINATED;

    //Require a context switch, as the task is finished;
    core_trigger_context_switch();

    //Infinite loop;
    while (true) {

        //TODO ERROR, FUNCTION CALLED FROM INTERRUPT OR CRITICAL SECTION LEAVE FORGOT IN THE CODE;

    }

}


//------------------------------------------ Runtime ------------------------------------------

//TODO SCHEDULER


/*
 * process_start_execution :
 *
 *  It then requires the user mode, and starts the execution of the first process_t;
 */

void process_start_execution() {

    //First, we must initialise the scheduler, that will configure threads before the actual start;
    scheduler_impl_initialise();

    //Get the first process to execute;
    process_t *first_process = scheduler_select_process();

    //TODO IN KERNEL INIT
    //Set the context switcher;
    core_set_context_switcher(&process_preempt);

    //TODO IN KERNEL INIT
    //Start the systick timer, and set the systick function;
    core_start_systick_timer(1, &systick_tick);

    //Enable the preemption in one millisecond;
    systick_set_process_duration(1);

    //Set the current stack pointer to the first thread's stack begin;;
    core_set_thread_stack_pointer(first_process->stack_begin);

    //Require the user mode;
    core_set_thread_mode();

    //Execute the first task;
    process_function(first_process);

}


/*
 * process_get_current : returns the process currently in execution;
 */

process_t *process_get_current() {

    //Returns the current process;
    return current_process;

}

//---------------------------- Context switch ----------------------------

/*
 * The context switcher :
 *
 *  - saves the current context;
 *  - calls the scheduler;
 *  - loads the new task's context;
 */

void process_preempt() {

    //Stack the context in the current process_t stack;
    core_stack_thread_context();

    //Save the current process_t's stack pointer, while the process_t hasn't been deleted;
    current_process->stack_pointer = core_get_thread_stack_pointer();

    //Update the current process's status (ex : if terminated, will be stopped);
    scheduler_cleanup_process(current_process);

    //Get a new process_t to execute, and pass the old one for eventual relocation;
    current_process = scheduler_select_process();

    //Set the next preemption time in 2 ms (1ms would leave potentially 0 ms);
    systick_set_process_duration(2);

    //Set the appropriate context;
    core_set_thread_stack_pointer(current_process->stack_pointer);

    //------------- FROM HERE, ALL DATA IS IN THE NEW TASK'S STACK -------------

    //Unstack the context from the previous process_t stack
    core_unstack_thread_context();

}




