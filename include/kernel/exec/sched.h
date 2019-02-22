/*
  sched.h Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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

/*
 * The sched library defines a general structure to implement a sched;
 *
 * 	A sched is in charge of selecting which task must be executed, referencing processes, sorting them, and
 * 	updating their state (active - inactive) if required;
 *
 * 	The sched comprises at least two list :
 * 	- the task list, containing all referenced processes;
 * 	- the active task list, containing all active processes. This list is kept sorted by decreasing order of
 * 		priority. Thanks to this invariant, the first available task is always the most prioritary;
 *
 *
 * 	It is potentially used in two different interrupt levels :
 * 	- preemption, where the previous task's context is saved, and a new task is selected;
 * 		This interrupt level has the lowest priority, as preemption must not happen during any interrupt;
 * 	- system, where processes are created, and their states are modified.
 *
 *
 * 	In order to maintain data integer, several precautions must be taken :
 * 	- the preemption level only has access to select and get functions; The kernel only has access to other functions;
 * 	- only the preemption level can manipulate the couple of lists;
 * 	- stopping a task, or terminating it will simply set flags, procedures themselves being executed in function
 * 		select;
 * 	- adding or resuming a task will result in the push of the task in a shared fifo, (@to_add or @to_activate).
 * 		Those processes will still be inactive, until they are read and scheduled by the select function;
 */




#include <list.h>

#include "pmem.h"

#include "prc.h"

#include <struct/shared_fifo.h>

struct scheduler {

	/*The first task; Leads to the task list;*/
	void *main_list;

	/*The first available task; Can be null. The linked list is sorted;*/
	void *active_list;

	/*The shared fifo, that contains tasks to activate;*/
	struct shared_fifo to_activate;

    /*TODO STATUS;*/
    
	/*If set the current task will be terminated at next @select call;*/
	bool termination_required;

	/*If set the current task will be stopped at next @select call;*/
	bool stop_required;

};


/*------------------------------------------------- Initialisation ------------------------------------------------*/

/*Construct a scheduler;*/
void sched_ctor(struct scheduler *sched, void *first_task_data);

/*Destruct a scheduler;*/
void sched_dtor(struct scheduler *sched);


/*----------------------------------------------------- Operations -----------------------------------------------------*/

/*Add a task to the scheduler;*/
void sched_add(void *task_data);

/*Terminate the current task; Will be at next select call;*/
void sched_terminate();


/*Set the current task in the stopped state, and return its ref;*/
struct sched_elmt *sched_stop();

/*Resume the provided task;*/
void sched_resume(struct sched_elmt *);



/*----------------------------------------------------- Commit -----------------------------------------------------*/

/*
 * Commit changes in the sched internal environment. This must be done after any manipulation;
 *
 * 									!!!!
 * This function must be called at the lowest interrupt priority level;
 * 									!!!!
 */

void sched_commit();


/*----------------------------------------------------- Query -----------------------------------------------------*/

/*Get the current task;*/
void *sched_get_task();

#endif /*TRACER_SCHEDULER_H*/
