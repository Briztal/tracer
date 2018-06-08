//
// Created by root on 5/5/18.
//

#ifndef TRACER_SCHEDULER_H
#define TRACER_SCHEDULER_H

#include "sprocess.h"


/*
 * ------------------------ Scheduler entry point ------------------------
 */

//Start the scheduler
void scheduler_start();

//Stop the scheduler;
void scheduler_stop();


/*
 * ------------------------ Standard scheduler functions ------------------------
 */

//Initialise the scheduling environment : called from the kernel at init;
void scheduler_create_sprocesses(size_t max_nb_processes);

//Update a sprocess after its preemption;
void scheduler_cleanup_sprocess(sprocess_t *);

//Clean a task;
void scheduler_cleanup_task(task_t *);

//Process reactivation : called from processes;
void scheduler_activate_sprocess(sprocess_t *);

//Process stop : called from preemption.
sprocess_t *scheduler_stop_current_sprocess();

//Select a sprocess to execute;
sprocess_t *scheduler_select_sprocess();


/*
 * ------------------------ Functions to implement for the project ------------------------
 */

//Initialisation of task manager and scheduler;
void scheduler_impl_initialise();

//Search in task containers for a task to execute;
task_t *scheduler_impl_get_task();

//Insert a sprocess in the provided list; called from preemption;
void scheduler_impl_insert_sprocess(linked_list_t *, sprocess_t *);

//Select the sprocess to be executed : called from preemption;
sprocess_t* scheduler_impl_select_sprocess(linked_list_t *);

extern task_t empty_task;


#endif //TRACER_SCHEDULER_H
