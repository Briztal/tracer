/*
  semaphore.c Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "semaphore.h"

#include "scheduler.h"

#include <kernel/kernel.h>


/*
 * Destructor : If the locked threads array is not empty, throws an exception.
 *
 *  If empty, deletes the locked-threads array;
 */

void sem_delete(semaphore_t *semaphore) {

    //TODO EXCEPTION ?

    //First, for all locked sprocesses :
    for (size_t unlock_index = semaphore->locked_threads.nb_elements; unlock_index--;) {

        //Get the sprocess to unlock;
        sprocess_t *sprocess = 
				*(sprocess_t **)container_get_element(&semaphore->locked_threads, unlock_index);

        //Re-activate the sprocess;
        scheduler_activate_sprocess(sprocess);

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
    if (!core_in_handler_mode()) {

        //If no execution is allowed :
        if (!semaphore->allocation_counter) {

            //Get our sprocess's index;
            stopped_process_id_t process = scheduler_stop_sprocess();

            //Add the sprocess pointer to the locked threads array;
            container_append_element(&semaphore->locked_threads, &process);

            //Trigger the context switch; Will be effective after the critical section exit;
            core_preempt_process();

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
    if (!core_in_handler_mode()) {

        //If threads are to unlock :
        if (semaphore->locked_threads.nb_elements) {

            //Get the sprocess to unlock;
            stopped_process_id_t process = *(sprocess_t **)container_get_element(&semaphore->locked_threads, 0);

            //Re-activate the sprocess;
            scheduler_activate_sprocess(process);

            //Remove the index of the array;
            container_remove_element(&semaphore->locked_threads, 0);

            //Trigger the context switch;
            core_preempt_process();

        } else {

            //If no threads are to unlock, increment the allocation counter;
            semaphore->allocation_counter++;

        }

    }

    //Leave the critical section, and eventually trigger the context switch;
    kernel_leave_critical_section();

}

