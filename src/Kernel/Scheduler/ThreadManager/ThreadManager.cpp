//
// Created by root on 4/2/18.
//


#include <DataStructures/Containers/ObjectContainer.h>
#include <Kernel/Scheduler/Systick.h>
#include <Kernel/Scheduler/TaskStorage.h>

#include "ThreadManager.h"

#include "Kernel/Scheduler/core_arm_cortex_m4f.h"

#include "ThreadData.h"

/*
 * Private variables and function for ThreadManager Library;
 */

namespace ThreadManager {


    //---------------------------- Private functions ----------------------------

    //Re initialise a thread (stack and context);
    void resetThread(ThreadData &thread_data, uint8_t thread_id);

    //Start a thread on the fly;
    void threadStart();

    //Threads main functions;
    void run(ThreadData *volatile data);

    //Threads termination functions;
    void termination_loop();

    //The Thread context switcher;
    void contextSwitcher();

    //---------------------------- Fields ----------------------------

    //Threads contexts;
    Container<ThreadData> threads(10);//TODO CONFIGURABLE

    //The current task;
    volatile uint8_t current_thread = 0;

}



/*
 * schedule : selects a new thread to run, and return its stack pointer;
 */

stack_ptr_t ThreadManager::schedule() {

    current_thread++;

    //Basic round robin.
    if (current_thread == threads.getSize())
        current_thread = 0;

    Systick::setTaskDuration(1);

    //Serial.println("Current Thread : " + String(current_thread));

    return (stack_ptr_t) (threads.getPointer(current_thread)->stack_pointer);

}


//---------------------------- Public functions ----------------------------


/*
 * initialise_thread : this function initialises a single thread;
 */

bool ThreadManager::createThread(const uint32_t stack_size) {

    //First, create a element in the threads array;
    threads.add(ThreadData());

    //Cache the thread's id;
    uint8_t thread_id = threads.getSize() - (uint8_t) 1;


    //Then, get a pointer to the thread's data;
    ThreadData *thread_data = threads.getPointer(threads.getSize() - (uint8_t) 1);

    //Then, allocate a memory zone of the required size;
    void *ptr = malloc(stack_size * sizeof(stack_element_t));

    //If the reallocation failed :
    if (ptr == nullptr) {
        Serial.println("Error during thread initialisation");
    }

    //If the malloc completed :

    Serial.println((long) thread_data);

    //Initialise the thread's stack data;
    stack_ptr_t sp = (stack_ptr_t) ptr;

    //Set the stack's begin;
    thread_data->stack_begin = core_get_stack_begin(sp, stack_size);

    //Set the stack's end;
    thread_data->stack_end = core_get_stack_end(sp, stack_size);

    Serial.println("THREAD ID" + String(thread_id));
    //Reset the thread in its initialisation state;
    resetThread(*thread_data, thread_id);

    //Complete;
    return true;

}


/*
 * setTask : this function is called by the scheduler to set the task of the given thread;
 */

void ThreadManager::setTask(uint8_t thread_index, uint8_t task_index) {

    //If the thread doesn't exist, fail;
    if (thread_index >= threads.getSize())
        return;//TODO KERNEL PANIC

    //Cache the current threads's data pointer;
    ThreadData *thread_ptr = threads.getPointer(thread_index);

    //If the thread is still in execution, fail;
    if (thread_ptr->current_state != TERMINATED)
        return;//TODO KERNEL PANIC;

    //Update the state;
    thread_ptr->current_state = (volatile thread_state) PENDING;

    //Update the thread's task;
    thread_ptr->task_index = task_index;

    Serial.println("Thread " + String(thread_index) + " updated");

}


/*
 * start : requires the user mode, and starts the execution of the first thread;
 */

void ThreadManager::start() {

    //Update the current thread;
    current_thread = 0;


    //TODO IN KERNEL INIT
    //Set the context switcher;
    core_set_context_switcher(&contextSwitcher);

    //TODO IN KERNEL INIT
    //Start the systick timer, and set the systick function;
    core_start_systick_timer(1, &Systick::systick);

    Serial.println("Systick Started, ");
    delay(2000);

    //Enable the preemtion in one millisecond;
    Systick::setTaskDuration(1);

    //Set the current stack pointer to the first thread's stack begin;;
    core_set_thread_stack_pointer(threads.getPointer(0)->stack_begin);

    //Require the user mode;
    core_set_thread_mode();

    //Execute the first task;
    run(threads.getPointer(0));

}


/*
 * set_current_task_sp : update the current task's stack pointer;
 */

void ThreadManager::set_current_task_sp(stack_ptr_t new_stack_pointer) {

    //Simply copy the new stack pointer in the appropriate thread data;
    threads.getPointer(current_thread)->stack_pointer = new_stack_pointer;

}


//---------------------------- Private functions ----------------------------

/*
 * resetThread : resets the thread to its initial stack pointer, and initialises the context to its beginning;
 */

void ThreadManager::resetThread(ThreadData &thread_data, const uint8_t thread_id) {

    //Declare a stack pointer;
    stack_ptr_t sp = thread_data.stack_begin;

    //Set the thread's stack pointer to the allocated memory zone;
    core_set_thread_stack_pointer(sp);

    //Initialise the stack;
    core_init_stack(threadStart, termination_loop, thread_id);

    //Get the new stack pointer;
    core_get_thread_stack_pointer(sp);

    //Save the new stack pointer;
    thread_data.stack_pointer = sp;

}


/*
 * threadStart : this function is called when a thread is started on the fly, without proper function call, due to
 *  a context switch;
 *
 *  As the thread ID has been saved in the register at the thread initialisation, the first step is to get it back;
 *
 *  Then, we can call properly the run function;
 */

void ThreadManager::threadStart() {

    //Declare the thread identifier;
    volatile stack_element_t thread_id = 0;

    //Define the thread identifier;
    core_get_thread_id(thread_id);

    //Run the thread;
    run(threads.getPointer(thread_id));

}


/*
 * run : this function is the base function of a thread;
 *
 *  It verifies that it can execute a task, then executes it if so.
 *
 *  Finally, it executes its termination function, and re-iterate;
 */

void ThreadManager::run(ThreadData *volatile data) {

    //Infinite loop;
    while (true) {

        //Serial.println("Thread "+String((uint32_t)data) +" "+ String(data->current_state == PENDING));

        //If the task must be executed :
        if ((volatile thread_state) data->current_state == PENDING) {

            //Mark the thread Active;
            data->current_state = (volatile thread_state) ACTIVE;

            //Cache the task index;
            volatile uint8_t task_index = data->task_index;

            //Cache the task data;
            volatile TaskData *volatile taskData = TaskStorage::getTask(task_index);

            //Execute the task;
            taskData->function(taskData->args);

            //Terminate the task;
            taskData->termination();

            //Remove the task;
            TaskStorage::removeTask(task_index);

            //Require a context switch, as the task is finished;
            core_trigger_context_switch();

        }

        //Mark the thread inactive;
        data->current_state = (volatile thread_state) TERMINATED;

        //Serial.println("TRIGGERIG d");

        //Systick::delay(500);

        //Serial.println("TRIGGERED");

    }

}


/*
 * termination_loop : does nothing but to wait;
 */

void ThreadManager::termination_loop() {
    while (true);
}




/*
 * The context switcher :
 *
 *  - saves the current contex;
 *  - calls the scheduler;
 *  - loads the new task's context;
 */

void ThreadManager::contextSwitcher() {

    //Stack the context in the current process stack;
    core_stack_thread_context();

    stack_ptr_t sp = nullptr;
    core_get_stack_pointer(sp);

    //Create a temp variable to contain the thread stack pointer;
    stack_ptr_t thread_stack_pointer = nullptr;

    //Get the stack pointer;
    core_get_thread_stack_pointer(thread_stack_pointer)

    //Serial.println("PSP : "+String((uint32_t)thread_stack_pointer));

    //Save the current thread's stack pointer;
    ThreadManager::set_current_task_sp(thread_stack_pointer);

    //Get the next working thread's stack pointer;
    stack_ptr_t stack_pointer = ThreadManager::schedule();

    //Serial.println("nPSP : "+String((uint32_t)stack_pointer));

    //Set the appropriate context;
    core_set_thread_stack_pointer(stack_pointer);

    //Unstack the context from the previous process stack
    core_unstack_thread_context();

}




