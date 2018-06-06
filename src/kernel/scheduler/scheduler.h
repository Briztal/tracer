//
// Created by root on 5/5/18.
//

#ifndef TRACER_SCHEDULER_H
#define TRACER_SCHEDULER_H

#include "process.h"


/*
 * ------------------------ Standard scheduler functions ------------------------
 */

//Initialise the scheduling environment : called from the kernel at init;
void scheduler_initialise(size_t max_nb_processes);

//Update a process after its preemption;
void scheduler_cleanup_process(process_t *);

//Process reactivation : called from threads;
void scheduler_activate_process(process_t *);

//Process stop : called from preemption.
void scheduler_stop_process(process_t *);

//Select a process to execute;
process_t *scheduler_select_process();


/*
 * ------------------------ Functions to implement for the project ------------------------
 */

//Initialisation of task manager and scheduler;
void scheduler_impl_initialise();

//Search in task containers for a task to execute;
task_t *scheduler_impl_get_task();

//Insert a process in the provided list; called from preemption;
void scheduler_impl_insert_process(linked_list_t *, process_t *);

//Select the process to be executed : called from preemption;
process_t* scheduler_impl_select_process(linked_list_t *);

extern task_t empty_task;


#endif //TRACER_SCHEDULER_H
