/*
  scheduler.c Part of TRACER

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

#include "scheduler.h"

#include <kernel/systick.h>

#include <kernel/kernel.h>

#include <kernel/scheduler/tasks/service.h>

#include <kernel/scheduler/tasks/sequences.h>


//---------------------------------- Private functions ----------------------------------

//Delete all sprocessed in all containers;
void scheduler_delete_sprocesses();

//Provide a new sprocess;
void scheduler_select_new_sprocess(core_process_t *);

//Transfer the sprocess from the pending list to the unregistered list
void scheduler_pending_to_stopped(sprocess_t *sprocess);

//Transfer the sprocess from the unregistered list to the pending list;
void scheduler_stopped_to_pending(sprocess_t *sprocess);

//Transfer the sprocess from the pending list to the terminated list
void scheduler_pending_to_terminated(sprocess_t *sprocess);


/*
 * The task to execute when the scheduler has no task to run;
 */

void scheduler_inactive_loop(void *ignored) {

    while (true);

}

void dumb_blink(void *);


//---------------------------------- Scheduler globals ----------------------------------

//The list of pending sprocesses;
static linked_list_t *pending_sprocesses;

//The list of unregistered sprocesses;
static linked_list_t *stopped_sprocesses;

//The list of terminated sprocesses;
static linked_list_t *terminated_sprocesses;

//The state of the scheduler;
static bool scheduler_initialised = false;

//The flag for inactive state;
static bool scheduler_inactive = false;

//The current sprocess in computation;
static sprocess_t *current_sprocess;


/*
 * The inactive mode task;
 */

stask_t empty_task = {
        .function = scheduler_inactive_loop,
        .cleanup = 0,
        .args = 0,
        .linked_element = EMPTY_LINKED_ELEMENT(),
        .task_type = EMPTY_STASK,
        .activity_time = 50,
};


/*
 * The blink task;
 */

stask_t blink_task = {
        .function = dumb_blink,
        .cleanup = 0,
        .args = 0,
        .linked_element = EMPTY_LINKED_ELEMENT(),
        .task_type = SERVICE_STASK,
        .activity_time = 2000
};


//---------------------------------- Init - Exit ----------------------------------

/*
 * scheduler_start : 
 *  - calls the implementation's initialization;
 * 	- updates the stack provider;
 * 	- selects and executed the first process;
 */

void scheduler_start(void (*init_function)(void *)) {

    //Disable the preemption;
    systick_set_process_duration(0);

    //First, we must initialise the scheduler, that will configure threads before the actual start;
    scheduler_impl_initialise();

    //Provide our stack provider to the core lib;
    core_set_process_provider(&scheduler_select_new_sprocess);

    //The init task must be scheduled via the sequencer (type 0 = exec asap), no args no cleanup, no preemption;
    sequences_add_task(0, init_function, 0, 0, 0);

    //Get the first sprocess to execute;
    current_sprocess = scheduler_select_sprocess();


    //Enable the preemption in 2 ms;
    systick_set_process_duration(current_sprocess->task->activity_time);

    //Execute the first process;
    sprocess_execute(current_sprocess);



    //While loop for safety. Will not happen, as previous call never returns;
    while (1);

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
    core_reset_process_provider();

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

uint16_t gf = 1;
volatile bool init = false;


void dumb_blink(void *unused) {

    volatile uint16_t delay = gf * (uint16_t) 30;

    gf *= 2;

    init = true;

    //Cache a pin configuration for pin C5 (LED);
    PORT_pin_config_t config;

    //Cache the LED pin;
    PORT_pin_t pin = {
            .port_data = &PORT_C,
            .bit_index = 5,
    };

    //Get the current configuration;
    PORT_get_pin_config(&pin, &config);

    //Update the configuration for led blink
    config.mux_channel = 1;
    config.direction = PORT_OUTPUT;
    config.output_mode = PORT_HIGH_DRIVE;

    //Update the configuration;
    PORT_set_pin_configuration(&pin, &config);

    //Create registers structs;
    GPIO_output_registers_t c_registers;

    //Get GPIO output registers for port C;
    PORT_get_GPIO_output_registers(&PORT_C, &c_registers);

    //Create the bitmask for C5;
    uint32_t mask = 1 << 5;

    //Indefinately :
    while (true) {

        //Turn on the LED;
        GPIO_set_bits(c_registers.set_register, mask);

        //Wait 10 ms;
        systick_wait(delay);

        //Turn off the LED;
        GPIO_clear_bits(c_registers.clear_register, mask);

        //Wait 10 ms;
        systick_wait(delay);

    }

}

 */

/*
 * scheduler_create_processes : initialises different linkes lists;
 */

void scheduler_create_sprocesses(size_t nb_processes) {

    if (scheduler_initialised) {
        kernel_error("scheduler.c : scheduler_create_sprocesses : the scheduler is already initialised;");
        return;
    }


    linked_list_t init = EMPTY_LINKED_LIST(nb_processes);

    //Initialise lists of sprocesses;
    pending_sprocesses = kernel_malloc_copy(sizeof(linked_list_t), &init);
    stopped_sprocesses = kernel_malloc_copy(sizeof(linked_list_t), &init);
    terminated_sprocesses = kernel_malloc_copy(sizeof(linked_list_t), &init);


    /*
     * All sprocesses have 2000 bytes of RAM;
     */
    for (; nb_processes--;) {

        //Create the sprocess in the heap with a 2k bytes long stack;
        sprocess_t *sprocess = sprocess_create(2000);

        /*
        sprocess->task = &blink_task;

        sprocess->state = SPROCESS_PENDING;

        sprocess->counter = ctr++;

        llist_insert_last(pending_sprocesses, (llist_t *) sprocess);

        //----------------------------------------------

        */




        //Add the process to the linked list;
        llist_insert_last(terminated_sprocesses, (llist_t *) sprocess);

    }

    //Mark the scheduler initialised;
    scheduler_initialised = true;

}


void scheduler_delete_sprocesses() {

    //TODO DELETE ALL

}

//---------------------------------- Scheduling ----------------------------------

/*
 * scheduler_select_new_sprocess : receives the old stack, selects a new process, updates the stack and returns the
 *  new time to preemption in milliseconds;
 */

void scheduler_select_new_sprocess(core_process_t *core_process) {

    //Update the current process's status (ex : if terminated, will be unregistered);
    scheduler_cleanup_sprocess(current_sprocess);

    //Get a new process_t to execute, update the current process;
    current_sprocess = scheduler_select_sprocess();

    //Return the stack of the new process;
    *core_process = (core_process_t) {
            .process_stack = &current_sprocess->stack,
            .activity_time = current_sprocess->task->activity_time,
    };

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

        //If the sprocess is in computation :
        case SPROCESS_PENDING:
            //Nothing to do, the process can be re-executed later;
            break;

        case SPROCESS_STOP_REQUIRED:
            //Stop the pending process;
            scheduler_pending_to_stopped(sprocess);
            break;

        case SPROCESS_STOPPED:
            //If the process is unregistered :

            kernel_error("scheduler.c : scheduler_cleanup_sprocess : attempted to clean a unregistered process;");

            break;

            //If the process has to be terminated :
        case SPROCESS_TERMINATION_REQUIRED:

            //Terminate the process;
            scheduler_pending_to_terminated(sprocess);

            break;

        case SPROCESS_TERMINATED:
            kernel_error("scheduler.c : scheduler_cleanup_sprocess : attempted to clean a terminated process;");
            break;

    }

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * scheduler_cleanup_task : receives an executed task and transmits it to its source storage module;
 */

void scheduler_cleanup_task(stask_t *task) {

    //Examine the type of the task;
    switch (task->task_type) {

        case EMPTY_STASK:
            //If the task is empty, nothing to do;
            break;

        case SEQUENCE_STASK:
            //If the task comes from a sequence, give it to the sequencer;
            sequences_remove_task((sequence_task_t *) task);
            break;

        case SERVICE_STASK:
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
    if (terminated_sprocesses->nb_elements) {

        //Query a task from task containers;
        stask_t *task = scheduler_impl_get_task();

        //If an available task was found :
        if (task) {

            //Remove the first terminated sprocess;
            sprocess_t *sprocess = (sprocess_t *) llist_remove_first(terminated_sprocesses);

            //Assign the sprocess's task;
            sprocess->task = task;

            //Insert the process in the pending list; Implementation call is made;
            scheduler_impl_insert_sprocess(pending_sprocesses, sprocess);

        }

    }


    /*
     * Select the next task to execute;
     */

    //Cache the next sprocess to execute;
    sprocess_t *sprocess;

    //If there are active processes :
    if (pending_sprocesses->nb_elements) {

        //Select the most urgent process to execute;
        sprocess = scheduler_impl_get_sprocess(pending_sprocesses);

        //The scheduler is active, and a non-empty process is running;
        scheduler_inactive = false;

    } else {

        //If there are no active sprocesses :

        //TODO WHAT IF ALL PROCESSES ARE STOPPED ????????

        //Remove the first terminated sprocess;
        sprocess = (sprocess_t *) llist_remove_first(terminated_sprocesses);

        //Insert it in the pending processes list;
        llist_insert_first(pending_sprocesses, (llist_t *) sprocess);

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

void scheduler_activate_sprocess(stopped_process_id_t stopped_process) {

    //If the provided pointer is null:
    if (!stopped_process) {

        //Error;
        kernel_error("scheduler.c : scheduler_activate_sprocess : received a null pointer;");

        return;

    }

    //First, cast the given pointer;
    sprocess_t *sprocess = (sprocess_t *) stopped_process;

    //Access to processes is critical;
    kernel_enter_critical_section();

    //Cache the state;
    sprocess_state_t state = sprocess->state;

    //Unlock the process_t if it is unregistered;
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
        kernel_error("scheduler.c : scheduler_activate_sprocess : attempted to activate a non-unregistered process;");

    }

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * scheduler_stop_sprocess : If possible, changes the state of the sprocess to STOP_REQUIRED.
 *  The scheduler will stop it effectively after context switch.
 */

stopped_process_id_t scheduler_stop_sprocess() {

    //Access to process is critical;
    kernel_enter_critical_section();

    //Lock the process_t if it is active;
    if (current_sprocess->state == SPROCESS_PENDING) {
        current_sprocess->state = SPROCESS_STOP_REQUIRED;
    } else {

        //Error;
        kernel_error("scheduler.c : scheduler_stop_sprocess : attempted to stop a non-pending process;");

    }

    //Leave the critical section;
    kernel_leave_critical_section();

    //Return the current process;
    return (stopped_process_id_t) current_sprocess;

}



//---------------------------------- List transfers ----------------------------------


/*
 * scheduler_stopped_to_pending : transfers a process from the unregistered list to the pending list;
 */

void scheduler_stopped_to_pending(sprocess_t *sprocess) {

    //Access to lists is critical;
    kernel_enter_critical_section();

    //Remove the sprocess from the unregistered list;
    llist_remove_element(stopped_sprocesses, (llist_t *) sprocess);

    //Insert the sprocess in the pending list; Implementation call is made;
    scheduler_impl_insert_sprocess(pending_sprocesses, sprocess);

    //For safety, set the sprocess state to unregistered;
    sprocess->state = SPROCESS_PENDING;

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * scheduler_pending_to_stopped : transfers a sprocess from the pending list to the unregistered list;
 */

void scheduler_pending_to_stopped(sprocess_t *sprocess) {

    //Access to lists is critical;
    kernel_enter_critical_section();

    //Remove the sprocess from the unregistered list;
    llist_remove_element(pending_sprocesses, (llist_t *) sprocess);

    //Insert the sprocess in the pending list; Implementation call is made;
    llist_insert_last(stopped_sprocesses, (llist_t *) sprocess);

    //For safety, set the sprocess state to unregistered;
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

    //Remove the sprocess from the unregistered list;
    llist_remove_element(pending_sprocesses, (llist_t *) sprocess);

    //Insert the sprocess in the pending list; Implementation call is made;
    llist_insert_last(terminated_sprocesses, (llist_t *) sprocess);

    //Leave the critical section;
    kernel_leave_critical_section();

}



