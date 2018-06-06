//
// Created by root on 5/24/18.
//

#include "scheduler.h"
#include "process.h"

#include <kernel/kernel.h>

#include <kernel/scheduler/tasks/service.h>

#include <kernel/scheduler/tasks/sequences.h>


//---------------------------------- Private functions ----------------------------------

//Transfer the process from the pending list to the stopped list
void scheduler_pending_to_stopped(process_t *process);

//Transfer the process from the stopped list to the pending list;
void scheduler_stopped_to_pending(process_t *process);

//Transfer the process from the pending list to the terminated list
void scheduler_pending_to_terminated(process_t *process);


/*
 * The task to execute when the scheduler has no task to run;
 */

void scheduler_inactive_loop(void * unused) {

    kernel_halt(2000);

    while(true);
}
//---------------------------------- Scheduler globals ----------------------------------

//The list of pending processes;
linked_list_t pending_processes;

//The list of stopped processes;
linked_list_t stopped_processes;

//The list of terminated processes;
linked_list_t terminated_processes;

//The state of the scheduler;
bool scheduler_initialised = false;

//The flag for inactive state;
bool scheduler_inactive = false;

/*
 * Declare the inactive mode task;
 */

task_t empty_task = {
        .function = scheduler_inactive_loop,
        .cleanup = 0,
        .args = 0,
        .linked_element = EMPTY_LINKED_ELEMENT(),
        .task_type = EMPTY_TASK,
};

//---------------------------------- Implementation ----------------------------------

/*
 * scheduler_initialise : initialises different linkes lists;
 */

void scheduler_initialise(size_t nb_processes) {


    if (scheduler_initialised) {
        //TODO ERROR;
        return;
    }

    //Initialise the list of processes;
    pending_processes = EMPTY_LINKED_LIST(nb_processes);
    stopped_processes = EMPTY_LINKED_LIST(nb_processes);
    terminated_processes = EMPTY_LINKED_LIST(nb_processes);

    /*
     * All processes have 1kb of RAM;
     */

    for (;nb_processes--;) {

        //Const initializer for a terminated process;
        const process_t init = {
                .link = EMPTY_LINKED_ELEMENT(),
                .task = 0,
                .state = PROCESS_TERMINATED,
                .stack_pointer = 0,
                .stack_begin = 0,
                .stack_end = 0,
        };

        //Create the process in the heap;
        process_t *process = kernel_malloc_copy(sizeof(process_t), &init);

        //Create the new process's context;
        process_create_context(process, 200);

        //Add the process to the linked list;
        llist_insert_end(&terminated_processes, (linked_element_t *) process);

    }

    //Mark the scheduler scheduler_initialised;
    scheduler_initialised = true;

}


/*
 * scheduler_activate_process : reactivates a process, called from threads;
 */

void scheduler_activate_process(process_t *process) {

    //Access to processes is critical;
    kernel_enter_critical_section();

    //Cache the state;
    process_state_t state = process->state;

    //Unlock the process_t if it is stopped;
    if (state == PROCESS_STOPPED) {

        //Set the state to active;
        process->state = PROCESS_PENDING;

        //Transfer the process;
        scheduler_stopped_to_pending(process);

    } else if (state == PROCESS_STOP_REQUIRED) {

        //If the stop has not been done yet, cancel it;
        process->state = PROCESS_PENDING;

    } else {

        //If the state is invalid for a reactivation, error;
        //TODO ERRO, ATTEMPTED TO ACTIVE A NON STOPPED PROCESS

    }

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * scheduler_stop_process : If possible, changes the state of the process to STOP_REQUIRED.
 *  The scheduler will stop it effectively after context switch.
 */

void scheduler_stop_process(process_t *process) {

    //Access to process is critical;
    kernel_enter_critical_section();

    //Lock the process_t if it is active;
    if (process->state == PROCESS_PENDING) {
        process->state = PROCESS_STOP_REQUIRED;
    } else {
        //TODO ERRO, ATTEMPTED TO STOP A NON ACTIVE PROCESS
    }

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * scheduler_stopped_to_pending : transfers a process from the stopped list to the pending list;
 */

void scheduler_stopped_to_pending(process_t *process) {

    //Access to lists is critical;
    kernel_enter_critical_section();

    //Remove the process from the stopped list;
    llist_remove_element(&stopped_processes, (linked_element_t *) process);

    //Insert the process in the pending list; Implementation call is made;
    scheduler_impl_insert_process(&pending_processes, process);

    //For safety, set the process state to stopped;
    process->state = PROCESS_PENDING;

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * scheduler_cleanup_process : called after a process is preempted.
 *  Must check its state, and change its storage list if required;
 */

void scheduler_cleanup_process(process_t *process) {

    //Access to process is critical;
    kernel_enter_critical_section();

    //If the scheduler is executing the empty task :
    if (scheduler_inactive) {

        //Mark the task terminated;
        process->state = PROCESS_TERMINATION_REQUIRED;

    }

    //If the process is terminated :
    switch (process->state) {

            //If the process is in execution :
        case PROCESS_PENDING:
            //Nothing to do, the process can be re-executed later;
            break;

        case PROCESS_STOP_REQUIRED:

            //Stop the pending process;
            scheduler_pending_to_stopped(process);

            break;

        case PROCESS_STOPPED:
            //If the process is stopped :

            //TODO ERROR CLEANED A STOPPED PROCESS;
            break;

            //If the process has to be terminated :
        case PROCESS_TERMINATION_REQUIRED:

            //Terminate the process;
            scheduler_pending_to_terminated(process);

            break;

        case PROCESS_TERMINATED:
            //TODO ERROR CLEANED A TERMINATED PROCESS;
            break;

    }

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * scheduler_pending_to_stopped : transfers a process from the pending list to the stopped list;
 */

void scheduler_pending_to_stopped(process_t *process) {

    //Access to lists is critical;
    kernel_enter_critical_section();

    //Remove the process from the stopped list;
    llist_remove_element(&pending_processes, (linked_element_t *) process);

    //Insert the process in the pending list; Implementation call is made;
    llist_insert_end(&stopped_processes, (linked_element_t *) process);

    //For safety, set the process state to stopped;
    process->state = PROCESS_STOPPED;

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * scheduler_pending_to_stopped : transfers a process from the pending list to the terminated list;
 */

void scheduler_pending_to_terminated(process_t *process) {

    //Access to lists is critical;
    kernel_enter_critical_section();

    //Cache the process's task pointer;
    task_t *task = process->task;

    //Examine the type of the task;
    switch(task->task_type) {

        case EMPTY_TASK:
            //If the task is empty, nothing to do;
            break;

        case SEQUENCE_TASK:
            //If the task comes from a sequence, give it to the sequencer;
            sequences_remove_task((sequence_task_t *) task);
            break;

        case SERVICE_TASK:
            //If the task comes from a service, give it to the service manager;
            services_reprogram((service_t *) task);
            break;

    }

    //Reset the process;
    process->task = 0;
    process->state = PROCESS_TERMINATED;
    process_reset_context(process);


    //Remove the process from the stopped list;
    llist_remove_element(&pending_processes, (linked_element_t *) process);

    //Insert the process in the pending list; Implementation call is made;
    llist_insert_end(&terminated_processes, (linked_element_t *) process);

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * scheduler_select_process :
 *  - Integrates tasks in the scheduler;
 *  - Queries the task to execute;
 *  - returns it if exists
 *  - Sleep mode if not;
 */

process_t *scheduler_select_process() {

    //Access to lists is critical;
    kernel_enter_critical_section();

    /*
     * Integrate tasks in the scheduler;
     */

    //If it is possible to integrate a task in the scheduler;
    if (terminated_processes.nb_elements) {

        //Query a task from task containers;
        task_t *task = scheduler_impl_get_task();

        //If an available task was found :
        if (task) {

            //Remove the first terminated process;
            process_t *process = (process_t *) llist_remove_begin(&terminated_processes);

            //Assign the process's task;
            process->task = task;

            //Insert the process in the pending list; Implementation call is made;
            scheduler_impl_insert_process(&pending_processes, process);

        }

    }


    /*
     * Select the next task to execute;
     */

    //Cache the next process to execute;
    process_t *process;

    //If there are active processes :
    if (pending_processes.nb_elements) {

        //Select the most urgent process to execute;
        process = scheduler_impl_select_process(&pending_processes);

        //The scheduler is active, and a non-empty process is running;
        scheduler_inactive = false;

    } else {
        //If there are no active processes :

        //Remove the first terminated process;
        process = (process_t *) llist_remove_begin(&terminated_processes);

        //Assign the empty task to the process;
        process->task = &empty_task;

        //The scheduler is inactive, it executes an empty process;
        scheduler_inactive = true;

        //TODO SLEEP MODE;

    }

    //Mark the process pending;
    process->state = PROCESS_PENDING;

    //Leave the critical section;
    kernel_leave_critical_section();


    //Return the process;
    return process;

}
