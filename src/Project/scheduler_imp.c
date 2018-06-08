//
// Created by root on 5/30/18.
//

#include <kernel/kernel.h>

#include <kernel/scheduler/scheduler.h>
#include <kernel/scheduler/tasks/service.h>
#include <kernel/scheduler/tasks/sequences.h>
#include <data_structures/containers/llist.h>

/*
 * scheduler_impl_initialise : initializes task managers and scheduler;
 */

void scheduler_impl_initialise() {

    //The scheduler will comprise 16 processes;
    scheduler_create_sprocesses(16);

    //The service manager will contain at most 255 simultaneous services;
    services_initialise(255);

    //The sequences manager will have 3 ordered sequences of size 16, and the 16 tasks unordered sequence;
    size_t sizes[3] = {16, 16, 16};
    sequences_initialise(16, 3, sizes);

}


/*
 * scheduler_impl_get_task : search in task containers for a task to execute;
 *
 *  This implementation searches in services first,
 *  then in sequences in increasing order (unordered sequence first);
 *
 *  If it finds no task, it returns 0;
 */

task_t *scheduler_impl_get_task() {

    //First, search for available services;
    if (services_available_task()) {

        //If services are available, get the first one;
        return services_get_task();

    } else {
        //If no service was found, examine sequences;

        //For each sequence starting by the unordered one :
        for (uint8_t sequence_index = 0; sequence_index < 4; sequence_index++) {

            //If a task is available in the sequence :
            if (sequences_available(sequence_index)) {

                //Return the sequence;
                return sequences_get_task(sequence_index);

            }

        }

    }

    //If no task was found, return 0
    return 0;

}


/*
 * scheduler_impl_insert_process : Inserts in the provided list; called from preemption;
 *
 *  This implementation is a simple round robin. It inserts the process at the end of the list;
 */

void scheduler_impl_insert_sprocess(linked_list_t *pending_sprocesses, sprocess_t *sprocess) {

    //Insert the process at the end of the list;
    llist_insert_end(pending_sprocesses, (linked_element_t *) sprocess);

}


/*
 * scheduler_impl_select_process : selects the process to be executed : called from preemption;
 *
 *  This implementation is a simple round robin. It removes and returns the first process of the list, if not empty.
 *
 *  If the list is empty, it returns 0 so that the scheduler can go in sleep mode;
 */

sprocess_t* scheduler_impl_select_sprocess(linked_list_t *pending_sprocesses) {

    //If the process list is empty, return 0 to trigger the sleep mode;
    if (!pending_sprocesses->nb_elements) {
        return 0;
    }

    //If the process list is not empty, remove and return the first process of the list;
    return (sprocess_t *) llist_remove_begin(pending_sprocesses);

}

