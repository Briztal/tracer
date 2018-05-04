//
// Created by root on 4/29/18.
//

#include <Kernel/Arch/Processors/core_arm_cortex_m4f.h>
#include <Kernel/Scheduler/ThreadManager/process.h>
#include <Kernel/Arch/Chips/teensy35/USBuart.h>

#include "Semaphore.h"

/*
 * Constructor : initialises the allocation counter and the locked threads array to its maximum capacity;
 */

Semaphore::Semaphore(uint16_t initial_counter) : allocation_counter(initial_counter), locked_threads(255) {

}


/*
 * Destructor : If the locked threads array is not empty, throws an exception.
 *
 *  If empty, deletes the locked-threads array;
 */

Semaphore::~Semaphore() {

    //TODO EXCEPTION;
    //TODO SET LOC
}


//TODO FIXED CONTAINER SIZE;


/*
 * P : required the execution;
 */

void Semaphore::P() {

    //As P is atomic, disable all interrupts;
    core_disable_interrupts();

    //If we are thread mode :
    if (core_in_thread_mode()) {

        //If no execution is allowed :
        if (!allocation_counter) {

            //Get our thread's index;
            uint8_t current_thread = ThreadManager::get_current_thread();

            //Lock our thread;
            ThreadManager::lock_thread(current_thread);

            //Add the thread index to the locked threads array;
            locked_threads.add(ThreadManager::get_current_thread());

            //Trigger the context switch;
            core_trigger_context_switch();

        } else {

            //If the execution is allowed, decrement the allocation counter;
            allocation_counter--;

        }

    }

    //Re-enable all interrupts, to effectively call context switch if triggered previously;
    core_enable_interrupts();

}


/*
 * V : allows one more execution;
 */

void Semaphore::V() {

    //As V is atomic, disable all interrupts;
    core_disable_interrupts();

    //If we are thread mode :
    if (core_in_thread_mode()) {

        //If threads are to unlock :
        if (locked_threads.getSize()) {

            //Get the index of the thread to unlock;
            uint8_t unlock_index = locked_threads.getElement(0);

            //Remove the index of the array;
            locked_threads.remove(0);

            //Unlock the thread;
            ThreadManager::unlock_thread(unlock_index);

            //Trigger the context switch;
            core_trigger_context_switch();

        } else {

            //If no threads are to unlock, increment the allocation counter;
            allocation_counter++;

        }

    }

    //Re-enable all interrupts, to effectively call context switch if triggered previously;
    core_enable_interrupts();

}
