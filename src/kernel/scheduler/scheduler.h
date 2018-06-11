/*
  scheduler.h Part of TRACER

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


#ifndef TRACER_SCHEDULER_H
#define TRACER_SCHEDULER_H

#include "sprocess.h"

//Stopped processes are given directly to semaphores and other sync objects;
//To avoid an unintentional access to process data from there programs, we will define a specific type;
typedef void *stopped_process_id_t;


/*
 * ------------------------ Scheduler entry point ------------------------
 */

//Start the scheduler
void scheduler_start();

//Stop the scheduler;
void scheduler_stop();


/*
 * ------------------------ Initialisation ------------------------
 */

//Initialise the scheduling environment : called from the kernel at init;
void scheduler_create_sprocesses(size_t max_nb_processes);


/*
 * ------------------------ State alteration ------------------------
 */

//Process reactivation : called from processes;
void scheduler_activate_sprocess(stopped_process_id_t);

//Process stop : called from preemption.
stopped_process_id_t scheduler_stop_sprocess();

//TODO TERMINATE_PROCESS


/*
 * ------------------------ Process selection ------------------------
 */

//Select a sprocess to execute;
sprocess_t *scheduler_select_sprocess();


/*
 * ------------------------ Process cleanup ------------------------
 */

//Update a sprocess after its preemption;
void scheduler_cleanup_sprocess(sprocess_t *);

//Clean a task;
void scheduler_cleanup_task(stask_t *);


/*
 * ------------------------ Functions to implement for the project ------------------------
 */

//Initialisation of task manager and scheduler;
void scheduler_impl_initialise();

//Search in task containers for a task to execute;
stask_t *scheduler_impl_get_task();

//Insert a sprocess in the provided list; called from preemption;
void scheduler_impl_insert_sprocess(linked_list_t *, sprocess_t *);

//Select the sprocess to be executed : called from preemption;
sprocess_t* scheduler_impl_get_sprocess(linked_list_t *);


#endif //TRACER_SCHEDULER_H
