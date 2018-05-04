//
// Created by root on 4/2/18.
//


#include "stdint.h"

#include "stdbool.h"

#include <DataStructures/Containers/ObjectContainer.h>
#include <Kernel/Scheduler/TaskStorage.h>
#include <Kernel/Scheduler/Schedulers/Scheduler.h>

#include "process.h"


//---------------------------- Private functions ----------------------------


//The function to start a thread on the fly;
void process_init();


//Threads main functions;
void process_function(volatile process *volatile process);

//Threads termination functions : infinite loop;
void process_exit() { while (1); }

//The process context switcher;
void process_preempt();


//---------------------------- Scheduler instance ----------------------------

//The instance of the scheduler, initialised at start time;
Scheduler *scheduler;


//---------------------------- Fields ----------------------------

//The current task;
volatile uint8_t current_thread = 0;

//Must the context switching be triggered ?
bool context_switch_triggered = false;

//The context switch semaphore. Context is effectively triggered when it is 0;
uint16_t context_switch_semaphore = 0;

//The thread mode flag;
bool threadMode = false;




//---------------------------- Process initialisation ----------------------------

/*
 * initialise_thread : this function allocates a stack, and then initialises stack pointers;
 *
 *  Finally, it sets the thread's state as Terminated;
 */

void process_create_context(process *process, uint32_t stack_size) {

    //Then, allocate a memory zone of the required size;
    void *ptr = malloc(stack_size * sizeof(stack_element_t));

    //If the reallocation failed :
    if (ptr == nullptr) {
        return;//TODO EXCEPTION;
    }

    //Initialise the thread's stack data;
    stack_ptr_t sp = (stack_ptr_t) ptr;

    //Set the stack's begin;
    process->stack_pointer = process->stack_begin = core_get_stack_begin(sp, stack_size);

    //Set the stack's end;
    process->stack_end = core_get_stack_end(sp, stack_size);

    //Reset the process context;
    process_reset_context(process);

}


/*
 * process_reset_context : resets the process to its initial stack pointer,
 * and initialises the context to its beginning;
 */

void process_reset_context(process * process) {

    //Declare a stack pointer;
    stack_ptr_t sp = process->stack_begin;

    //Set the thread's stack pointer to the allocated memory zone;
    core_set_thread_stack_pointer(sp);

    //Initialise the stack;
    core_init_stack(process_init, process_exit, (stack_element_t) process);

    //Save the new stack pointer;
    process->stack_pointer = core_get_thread_stack_pointer();

    //As the process is just reset, he potentially have no function to execute. It is so terminated by default;
    process->state = TERMINATED;

}


//---------------------------- State alteration ----------------------------

/*
 * process_lock : locks the required process;
 */

void process_lock(process *process) {

    //Lock the process if it is active;
    if (process->state == ACTIVE)
        process->state = STOPPED;

}


/*
 * process_unlock : unlocks the required process;
 */

void process_unlock(process *process) {

    //Unlock the process if it is stopped;
    if (process->state == STOPPED)
        process->state = ACTIVE;

}


/*
 * process_init : this function is called when a process is started on the fly, without proper function call, due to
 *  a context switch;
 *
 * TODO CHANGE THIS SHIT ! INITIALISE ALL THREADS AND EXECUTE THREAD 0 WITH A LONGJMP
 *
 *  As the process pointer has been saved in the register at the thread initialisation, the first step is to get it back;
 *
 *  Then, we can call properly the run function;
 */

void process_init() {

    //Get the thread index and run the associated thread;
    process_function((volatile process *volatile) core_get_process());

}


/*
 * process_function : this function is the routine executed by a process;
 */

void process_function(volatile process *volatile process) {

    //The process must be pending, otherwise, there has been an error in the scheduler.
    if ((volatile process_state) process->state != PENDING) {

        //Kernel panic;
        //TODO KERNEL PANIC;

    }
    //Mark the process active;
    process->state = (volatile process_state) ACTIVE;

    //Execute the function, passing args;
    (*(process->process_function))(process->args);

    //Execute the exit function;
    (*(process->exit_function))();

    //Mark the process inactive; Scheduler will be notified at context switch;
    process->state = (volatile process_state) TERMINATED;

    //Infinite loop;
    while (true) {

        //Require a context switch, as the task is finished;
        core_trigger_context_switch();

    }

}


//------------------------------------------ Execution ------------------------------------------

//TODO SCHEDULER


/*
 * process_start_execution :
 *
 *  It then requires the user mode, and starts the execution of the first process;
 */

void process_start_execution() {

    //First, get the scheduler;
    Scheduler * project_scheduler = Project::instantiate_scheduler();

    //Initialise our copy of the scheduler pointer;
    ThreadManager::scheduler = project_scheduler;

    //First, we must initialise the scheduler, that will configure threads before the actual start;
    project_scheduler->initialise();

    //Update the current thread;
    current_thread = 0;

    //TODO IN KERNEL INIT
    //Set the context switcher;
    core_set_context_switcher(&threads_context_switcher);

    //TODO IN KERNEL INIT
    //Start the systick timer, and set the systick function;
    core_start_systick_timer(1, &Systick::systick);

    Serial.println("Systick Started, ");
    //TODO sleep(2000);

    //Enable the preemtion in one millisecond;
    Systick::setTaskDuration(1);

    //Set the current stack pointer to the first thread's stack begin;;
    core_set_thread_stack_pointer(threads.getPointer(current_thread)->stack_begin);

    //Require the user mode;
    core_set_thread_mode();

    //Execute the first task;
    run(threads.getPointer(current_thread));

}


//---------------------------- Context switch ----------------------------

process *current_process;


/*
 * The context switcher :
 *
 *  - saves the current context;
 *  - calls the scheduler;
 *  - loads the new task's context;
 */

void process_preempt() {

    //Stack the context in the current process stack;
    core_stack_thread_context();

    //Save the current process's stack pointer, while the process hasn't been deleted;
    current_process->stack_pointer = core_get_thread_stack_pointer();

    //Get a new process to execute, and pass the old one for eventual relocation;
    current_process = scheduler->schedule(current_process),

            //Set the appropriate context;
            core_set_thread_stack_pointer(current_process->stack_pointer);

    //------------- FROM HERE, ALL DATA IS IN THE NEW TASK'S STACK -------------

    //Unstack the context from the previous process stack
    core_unstack_thread_context();

}




