//
// Created by root on 4/29/18.
//

#include "semaphore.h"

#include <kernel/kernel.h>

#include "kernel/scheduler/process.h"
#include "scheduler.h"

/*
 * Destructor : If the locked threads array is not empty, throws an exception.
 *
 *  If empty, deletes the locked-threads array;
 */

void sem_delete(semaphore_t *semaphore) {

    //TODO EXCEPTION ?

    //First, for all locked processes :
    for (size_t unlock_index = semaphore->locked_threads.nb_elements; unlock_index--;) {

        //Get the process to unlock;
        process_t *process = *(process_t **)container_get_element(&semaphore->locked_threads, unlock_index);

        //Re-activate the process;
        scheduler_activate_process(process);

    }

    //Then, free the container's data;
    kernel_free(semaphore->locked_threads.elements);

    //Finally, free the semaphore;
    kernel_free(semaphore);


}


/*
 * sem_wait : required the execution;
 */

void sem_wait(semaphore_t *semaphore) {

    //The semaphore function is critical;
    kernel_enter_critical_section();

    //If we are thread mode :
    if (core_in_thread_mode()) {

        //If no execution is allowed :
        if (!semaphore->allocation_counter) {

            //Get our process's index;
            process_t *process = process_get_current();

            //Stop the process;
            scheduler_stop_process(process);

            //Add the process pointer to the locked threads array;
            container_append_element(&semaphore->locked_threads, &process);

            //Trigger the context switch; Will be effective after the critical section exit;
            core_trigger_context_switch();

        } else {

            //If the execution is allowed, decrement the allocation counter;
            semaphore->allocation_counter--;

        }

    }

    //Leave the critical section, and eventually trigger the context switch;
    kernel_leave_critical_section();

}


/*
 * sem_post : allows one more execution;
 */

void sem_post(semaphore_t *semaphore) {

    //The semaphore function is critical;
    kernel_enter_critical_section();

    //If we are thread mode :
    if (core_in_thread_mode()) {

        //If threads are to unlock :
        if (semaphore->locked_threads.nb_elements) {

            //Get the process to unlock;
            process_t *process = *(process_t **)container_get_element(&semaphore->locked_threads, 0);

            //Re-activate the process;
            scheduler_activate_process(process);

            //Remove the index of the array;
            container_remove_element(&semaphore->locked_threads, 0);


            //Trigger the context switch;
            core_trigger_context_switch();

        } else {

            //If no threads are to unlock, increment the allocation counter;
            semaphore->allocation_counter++;

        }

    }

    //Leave the critical section, and eventually trigger the context switch;
    kernel_leave_critical_section();


}




