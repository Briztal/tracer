//
// Created by root on 4/2/18.
//


#include <DataStructures/Containers/ObjectContainer.h>
#include "ThreadManager.h"

#include "Arduino.h"

#include "core_arm_cortex_m4f.h"
#include "ThreadData.h"


//First, let's determine the number of threads;

const uint8_t NB_THREADS = (uint8_t) 0

#define THREAD(i, ...) + ((uint8_t) 1)

#include "threads_config.h"

;

#undef THREAD





/*
 * Private variables and function for ThreadManager Library;
 */

namespace ThreadManager {

    //Start a thread on the fly;
    void threadStart();

    //Threads main functions;
    void run(uint8_t);

    //Threads termination functions;
    void termination_loop();

    //Threads contexts;
    static ObjectContainer<ThreadData> threads;

    //The current task;
    static volatile uint8_t current_thread = 0;

    void resetThread(ThreadData &thread_data, uint8_t thread_id);
}


void ThreadManager::addTask(TaskData &task) {

    Serial.println("ADDTASK ");

    const uint8_t nb_threads = threads.getSize();

    for (uint8_t i = 0; i < nb_threads; i++) {

        if (threads_data[i].current_state == TERMINATED) {

            threads_data[i].current_state = (volatile thread_state)PENDING;

            threads_data[i].task = new TaskData(task);

            //Serial.println("PTR : "+String((uint32_t) threads_data[i].task));

            //Serial.println(String((long)task.function)+" "+String((long)&threads_data[i].task->function));

            Serial.println("SAVED IN THREAD "+String(i));

            return;

        }


    }


}


/*
 * initialise_thread : this function initialises a single thread;
 */

bool ThreadManager::createThread(uint32_t stack_size) {

    //First, create a heap copy of the thread data and add it to the threads array;
    if (threads.add(ThreadData()));//TODO ADD THREAD

    //Then, allocate a memory zone of the required size;
    void *ptr = malloc(stack_size * sizeof(stack_element_t));

    //If the reallocation failed :
    if (ptr == nullptr) {
        Serial.println("Error during thread initialisation");
    }

    //If the malloc completed :

    //Initialise the thread's data;

    //Complete;
    return true;

}


/*
 * resetThread : resets the thread to its initial stack pointer, and initialises the context to its beginning;
 */

void ThreadManager::resetThread(ThreadData &thread_data, uint8_t thread_id) {

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
 * trigger_context_switch : enables the context switching at next thread exit;
 */

inline void trigger_context_switch() {

    //Call the core function;
    core_trigger_context_switch();

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
    run((uint8_t)thread_id);

}

/*
 * run : this function is the base function of a thread;
 *
 *  It verifies that it can execute a task, then executes it if so.
 *
 *  Finally, it executes its termination function, and re-iterate;
 */

void ThreadManager::run(volatile uint8_t thread_id) {

    //Infinite loop;
    while (true) {

        Serial.println("Thread "+String((uint32_t)thread_id) +" "+ String(threads_data[thread_id].current_state == PENDING));

        //If the task must be executed :
        if (threads_data[thread_id].current_state == PENDING) {

            //Mark the thread Active;
            threads_data[thread_id].current_state = (volatile thread_state)ACTIVE;

            //Cache the task;
            volatile TaskData *task_p = threads_data[thread_id].task;

            //Execute the task;
            task_p->function(task_p->args);

            //Terminate the task;
            task_p->termination();

        }

        //Mark the thread inactive;
        threads_data[thread_id].current_state = (volatile thread_state)TERMINATED;

        Serial.println("TRIGGERIG d");

        delay(500);

        //Require a context switch, as the task is finished;
        trigger_context_switch();

        Serial.println("TRIGGERED");

    }

}


/*
 * termination_loop : does nothing but to wait;
 */

void ThreadManager::termination_loop() {
    while (true);
}



/*
 * The context switcher;
 */


void pendSV_Handler() {

    Serial.println("PENDSV");

    //Stack the context in the current process stack;
    core_stack_thread_context();

    uint32_t sp = 0;
    core_get_stack_pointer(sp);

    Serial.println("SP : "+String(sp));

    //Create a temp variable to contain the thread stack pointer;
    stack_ptr_t thread_stack_pointer = 0;

    //Get the stack pointer;
    core_get_thread_stack_pointer(thread_stack_pointer)

    Serial.println("PSP : "+String((uint32_t)thread_stack_pointer));

    //Save the current thread's stack pointer;
    ThreadManager::set_current_task_sp(thread_stack_pointer);

    //Get the next working thread's stack pointer;
    stack_ptr_t stack_pointer = ThreadManager::schedule();

    Serial.println("nPSP : "+String((uint32_t)stack_pointer));

    //Set the appropriate context;
    core_set_thread_stack_pointer(stack_pointer);

    //Unstack the context from the previous process stack
    core_unstack_thread_context();

}


/*
 * start : requires the user mode, and starts the execution of the first thread;
 */

void ThreadManager::start() {

    //Update the current thread;
    current_thread = 0;

    //TODO PROPER CORE INIT;
    _VectorsRam[14] = &pendSV_Handler;
    NVIC_SET_PRIORITY(-2, 240);

    //Set the current stack pointer;
    core_set_thread_stack_pointer(threads_data[0].stack_pointer + 8);

    //Require the user mode;
    core_set_thread_mode();

    //Execute the first task;
    run(0);


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

    current_thread++;

    //Basic round robin.
    if (current_thread == NB_THREADS)
        current_thread = 0;

    Serial.println("Current Thread : "+String(current_thread));

    return (stack_ptr_t) (threads_data[current_thread].stack_pointer);

}

