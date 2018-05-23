//
// Created by root on 4/29/18.
//

#include <data_structures/containers/container.h>
#include <malloc.h>
#include "semaphore.h"

#include "Kernel/arch/arch_headers.h"

#include "Kernel/scheduler/process.h"


void semaphore_unlock(semaphore_t *semaphore) {

    //Get the process to unlock;
    process_t *process = *(process_t **)container_get_element(&semaphore->locked_threads, 0);

    //Unlock the process;
    process_unlock(process);

    //Remove the index of the array;
    container_remove_element(&semaphore->locked_threads, 0);

}

/*
 * Destructor : If the locked threads array is not empty, throws an exception.
 *
 *  If empty, deletes the locked-threads array;
 */

void sem_delete(semaphore_t *semaphore) {

    //TODO EXCEPTION ?

    //First, for all locked processes :
    for (container_index_t unlock_index = semaphore->locked_threads.nb_elements; unlock_index--;) {

        //Get the process to unlock;
        process_t *process = *(process_t **)container_get_element(&semaphore->locked_threads, unlock_index);

        //Unlock the process;
        process_unlock(process);

    }

    //Then, free the container's data;
    free(semaphore->locked_threads.elements);

    //Finally, free the semaphore;
    free(semaphore);


}


/*
 * sem_wait : required the execution;
 */

void sem_wait(semaphore_t *semaphore) {

    //As P is atomic, disable all interrupts;
    core_disable_interrupts();

    //If we are thread mode :
    if (core_in_thread_mode()) {

        //If no execution is allowed :
        if (!semaphore->allocation_counter) {

            //Get our process's index;
            process_t *process = process_get_current();

            //Lock the process;
            process_lock(process);

            //Add the process pointer to the locked threads array;
            container_append_element(&semaphore->locked_threads, &process);

            //Trigger the context switch;
            core_trigger_context_switch();

        } else {

            //If the execution is allowed, decrement the allocation counter;
            semaphore->allocation_counter--;

        }

    }

    //Re-enable all interrupts, to effectively call context switch if triggered previously;
    core_enable_interrupts();

}


/*
 * sem_post : allows one more execution;
 */

void sem_post(semaphore_t *semaphore) {

    //As V is atomic, disable all interrupts;
    core_disable_interrupts();

    //If we are thread mode :
    if (core_in_thread_mode()) {

        //If threads are to unlock :
        if (semaphore->locked_threads.nb_elements) {

            //Get the process to unlock;
            process_t *process = *(process_t **)container_get_element(&semaphore->locked_threads, 0);

            //Unlock the process;
            process_unlock(process);

            //Remove the index of the array;
            container_remove_element(&semaphore->locked_threads, 0);


            //Trigger the context switch;
            core_trigger_context_switch();

        } else {

            //If no threads are to unlock, increment the allocation counter;
            semaphore->allocation_counter++;

        }

    }

    //Re-enable all interrupts, to effectively call context switch if triggered previously;
    core_enable_interrupts();

}




