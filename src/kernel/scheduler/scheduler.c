//
// Created by root on 5/24/18.
//

#include "scheduler.h"

#include "sprocess.h"

#include <kernel/systick.h>

#include <kernel/kernel.h>

#include <kernel/scheduler/tasks/service.h>

#include <kernel/scheduler/tasks/sequences.h>


//---------------------------------- Private functions ----------------------------------

//Delete all sprocessed in all containers;
void scheduler_delete_sprocesses();	

//Provide a new sprocess;
stack_t * scheduler_select_new_sprocess(stack_t *);

//Transfer the sprocess from the pending list to the stopped list
void scheduler_pending_to_stopped(sprocess_t *sprocess);

//Transfer the sprocess from the stopped list to the pending list;
void scheduler_stopped_to_pending(sprocess_t *sprocess);

//Transfer the sprocess from the pending list to the terminated list
void scheduler_pending_to_terminated(sprocess_t *sprocess);


/*
 * The task to execute when the scheduler has no task to run;
 */

void scheduler_inactive_loop(void * unused) {
	
	systick_set_process_duration(0);	

    kernel_halt(2000);

    while(true);
}


//---------------------------------- Scheduler globals ----------------------------------

//The list of pending sprocesses;
linked_list_t pending_sprocesses;

//The list of stopped sprocesses;
linked_list_t stopped_sprocesses;

//The list of terminated sprocesses;
linked_list_t terminated_sprocesses;

//The state of the scheduler;
bool scheduler_initialised = false;

//The flag for inactive state;
bool scheduler_inactive = false;

//The current sprocess in execution;
sprocess_t *current_sprocess;

/*
 * The inactive mode task;
 */

task_t empty_task = {
        .function = scheduler_inactive_loop,
        .cleanup = 0,
        .args = 0,
        .linked_element = EMPTY_LINKED_ELEMENT(),
        .task_type = EMPTY_TASK,
};


//---------------------------------- Init - Exit ----------------------------------

/*
 * scheduler_start : 
 *  - calls the implementation's initialization;
 * 	- updates the stack provider;
 * 	- selects and executed the first process;
 */

void scheduler_start() {
    
	//Disable the preemption;
	systick_set_process_duration(0);

	//First, we must initialise the scheduler, that will configure threads before the actual start;
    scheduler_impl_initialise();

	//Provide our stack provider to the core lib;
	core_set_stack_provider(&scheduler_select_new_sprocess);

    //Get the first sprocess to execute;
    current_sprocess = scheduler_select_sprocess();
 	
	/*
    if (current_sprocess->task == &empty_task) {

        kernel_halt(200);

    } else {

        kernel_halt(2000);

    }
	*/

	//Enable the preemption in 2 ms;
	systick_set_process_duration(2);

	//Execute the first process; 
   	sprocess_execute(current_sprocess);
 
	//While loop for safety. Will not happen, as previous call never returns;
	while(1);

}


/*
 * scheduler_stop : stops the scheduler : 
 * 	- disables the preemption;
 * 	- clean all process containers and delete all processes;
 * 	- reset the core stack provider;
 */

void scheduler_stop() {

    //Disable the preemption;
    systick_set_process_duration(0);

    //TODO EFFECTIVELY DELETE THREADS;
	scheduler_delete_sprocesses();

	//Reset the stack provider;
	core_reset_stack_provider();

    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::
    //TODO DISABLE THE CONTEXT SWITCHING ::::::::::::::::::::


}


/*
 * scheduler_create_processes : initialises different linkes lists;
 */

void scheduler_create_sprocesses(size_t nb_processes) {

    if (scheduler_initialised) {
        //TODO ERROR;
        return;
    }

    //Initialise lists of sprocesses;
    pending_sprocesses = EMPTY_LINKED_LIST(nb_processes);
    stopped_sprocesses = EMPTY_LINKED_LIST(nb_processes);
    terminated_sprocesses = EMPTY_LINKED_LIST(nb_processes);

    /*
     * All sprocesses have 200 bytes of RAM;
     */

    for (;nb_processes--;) {
        
		//Create the sprocess in the heap with a 200 bytes long stack;
        sprocess_t *sprocess = sprocess_create(200);

        //Add the process to the linked list;
        llist_insert_end(&terminated_sprocesses, (linked_element_t *) sprocess);

    }

    //Mark the scheduler scheduler_initialised;
    scheduler_initialised = true;

}

void scheduler_delete_sprocesses() {
	
	//TODO DELETE ALL

}

//---------------------------------- Scheduling ----------------------------------

/*
 * scheduler_select_new_sprocess : receives the old stack, selects a new process, returns its stack;
 */

stack_t *scheduler_select_new_sprocess(stack_t *old_stack) {

    //Update the current process's status (ex : if terminated, will be stopped);
    scheduler_cleanup_sprocess(current_sprocess);

    //Get a new process_t to execute, update the current process;
    current_sprocess = scheduler_select_sprocess();

	//Return the stack of the new process;
	return &current_sprocess->stack;

}


/*
 * scheduler_cleanup_sprocess : called after a process is preempted.
 *  Must check its state, and change its storage list if required;
 */

void scheduler_cleanup_sprocess(sprocess_t *sprocess) {

    //Access to sprocess is critical;
    kernel_enter_critical_section();

    //If the scheduler is executing the empty task :
    if (scheduler_inactive) {

        //Mark the task terminated;
        sprocess->state = SPROCESS_TERMINATION_REQUIRED;

    }

    switch (sprocess->state) {

            //If the sprocess is in execution :
        case SPROCESS_PENDING:
            //Nothing to do, the process can be re-executed later;
            break;

        case SPROCESS_STOP_REQUIRED:
            //Stop the pending process;
            scheduler_pending_to_stopped(sprocess);
            break;

        case SPROCESS_STOPPED:
            //If the process is stopped :

            //TODO ERROR CLEANED A STOPPED PROCESS;
            break;

            //If the process has to be terminated :
        case SPROCESS_TERMINATION_REQUIRED:

            //Terminate the process;
            scheduler_pending_to_terminated(sprocess);

            break;

        case SPROCESS_TERMINATED:
            //TODO ERROR CLEANED A TERMINATED PROCESS;
            break;

    }

    //Leave the critical section;
    kernel_leave_critical_section();

}

/*
 * scheduler_cleanup_task : receives an executed task and transmits it to its source storage module;
 */

void scheduler_cleanup_task(task_t *task) {
	
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

}


/*
 * scheduler_select_sprocess :
 *  - Integrates tasks in the scheduler;
 *  - Queries the task to execute;
 *  - returns it if exists
 *  - Sleep mode if not;
 */

sprocess_t *scheduler_select_sprocess() {

    //Access to lists is critical;
    kernel_enter_critical_section();

    /*
     * Integrate tasks in the scheduler;
     */

    //If it is possible to integrate a task in the scheduler;
    if (terminated_sprocesses.nb_elements) {

        //Query a task from task containers;
        task_t *task = scheduler_impl_get_task();

        //If an available task was found :
        if (task) {

            //Remove the first terminated sprocess;
            sprocess_t *sprocess = (sprocess_t *) llist_remove_begin(&terminated_sprocesses);
			
            //Assign the sprocess's task;
            sprocess->task = task;

            //Insert the process in the pending list; Implementation call is made;
            scheduler_impl_insert_sprocess(&pending_sprocesses, sprocess);

        }

    }


    /*
     * Select the next task to execute;
     */

    //Cache the next sprocess to execute;
    sprocess_t *sprocess;

    //If there are active processes :
    if (pending_sprocesses.nb_elements) {

        //Select the most urgent process to execute;
        sprocess = scheduler_impl_select_sprocess(&pending_sprocesses);

        //The scheduler is active, and a non-empty process is running;
        scheduler_inactive = false;

    } else {
        //If there are no active sprocesses :

        //Remove the first terminated sprocess;
        sprocess = (sprocess_t *) llist_remove_begin(&terminated_sprocesses);

        //Assign the empty task to the sprocess;
        sprocess->task = &empty_task;

        //The scheduler is inactive, it executes an empty sprocess;
        scheduler_inactive = true;

        //TODO SLEEP MODE;

    }

    //Mark the sprocess pending;
    sprocess->state = SPROCESS_PENDING;

    //Leave the critical section;
    kernel_leave_critical_section();


    //Return the sprocess;
    return sprocess;

}


//---------------------------------- State settings ----------------------------------

/*
 * scheduler_activate_sprocess : reactivates a sprocess, called from threads;
 */

void scheduler_activate_sprocess(sprocess_t *sprocess) {

    //Access to processes is critical;
    kernel_enter_critical_section();

    //Cache the state;
    sprocess_state_t state = sprocess->state;

    //Unlock the process_t if it is stopped;
    if (state == SPROCESS_STOPPED) {

        //Set the state to active;
        sprocess->state = SPROCESS_PENDING;

        //Transfer the process;
        scheduler_stopped_to_pending(sprocess);

    } else if (state == SPROCESS_STOP_REQUIRED) {

        //If the stop has not been done yet, cancel it;
        sprocess->state = SPROCESS_PENDING;

    } else {

        //If the state is invalid for a reactivation, error;
        //TODO ERRO, ATTEMPTED TO ACTIVE A NON STOPPED PROCESS

    }

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * scheduler_stop_sprocess : If possible, changes the state of the sprocess to STOP_REQUIRED.
 *  The scheduler will stop it effectively after context switch.
 */

sprocess_t *scheduler_stop_current_sprocess() {

    //Access to process is critical;
    kernel_enter_critical_section();

    //Lock the process_t if it is active;
    if (current_sprocess->state == SPROCESS_PENDING) {
        current_sprocess->state = SPROCESS_STOP_REQUIRED;
    } else {
        //TODO ERROR, THE CURRENT PROCESS IS NOT IN THE PENDING STATE; 
    }

    //Leave the critical section;
    kernel_leave_critical_section();
	
	//Return the current process;
	return current_sprocess;	

}


//---------------------------------- List transfers ----------------------------------


/*
 * scheduler_stopped_to_pending : transfers a process from the stopped list to the pending list;
 */

void scheduler_stopped_to_pending(sprocess_t *sprocess) {

    //Access to lists is critical;
    kernel_enter_critical_section();

    //Remove the sprocess from the stopped list;
    llist_remove_element(&stopped_sprocesses, (linked_element_t *) sprocess);

    //Insert the sprocess in the pending list; Implementation call is made;
    scheduler_impl_insert_sprocess(&pending_sprocesses, sprocess);

    //For safety, set the sprocess state to stopped;
    sprocess->state = SPROCESS_PENDING;

    //Leave the critical section;
    kernel_leave_critical_section();

}

/*
 * scheduler_pending_to_stopped : transfers a sprocess from the pending list to the stopped list;
 */

void scheduler_pending_to_stopped(sprocess_t *sprocess) {

    //Access to lists is critical;
    kernel_enter_critical_section();

    //Remove the sprocess from the stopped list;
    llist_remove_element(&pending_sprocesses, (linked_element_t *) sprocess);

    //Insert the sprocess in the pending list; Implementation call is made;
    llist_insert_end(&stopped_sprocesses, (linked_element_t *) sprocess);

    //For safety, set the sprocess state to stopped;
    sprocess->state = SPROCESS_STOPPED;

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * scheduler_pending_to_stopped : transfers a sprocess from the pending list to the terminated list;
 */

void scheduler_pending_to_terminated(sprocess_t *sprocess) {

    //Access to lists is critical;
    kernel_enter_critical_section();
   
	//Reset the sprocess;
	sprocess_reset(sprocess);

    //Remove the sprocess from the stopped list;
    llist_remove_element(&pending_sprocesses, (linked_element_t *) sprocess);

    //Insert the sprocess in the pending list; Implementation call is made;
    llist_insert_end(&terminated_sprocesses, (linked_element_t *) sprocess);

    //Leave the critical section;
    kernel_leave_critical_section();

}


