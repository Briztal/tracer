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

/*
 * The scheduler library defines a general structure to implement a scheduler;
 *
 * 	A scheduler is in charge of selecting which process must be executed, referencing processes, sorting them, and
 * 	updating their state (active - inactive) if required;
 *
 * 	The scheduler comprises at least two list :
 * 	- the process list, containing all referenced processes;
 * 	- the active process list, containing all active processes. This list is kept sorted by decreasing order of
 * 		priority. Thanks to this invariant, the first available process is always the most prioritary;
 *
 *
 * 	It is potentially used in two different interrupt levels :
 * 	- preemption, where the previous process's context is saved, and a new process is selected;
 * 		This interrupt level has the lowest priority, as preemption must not happen during any interrupt;
 * 	- system, where processes are created, and their states are modified.
 *
 *
 * 	In order to maintain data integer, several precautions must be taken :
 * 	- the preemption level only has access to select and get functions; The kernel only has access to other functions;
 * 	- only the preemption level can manipulate the couple of lists;
 * 	- stopping a process, or terminating it will simply set flags, procedures themselves being executed in function
 * 		select;
 * 	- adding or resuming a process will result in the push of the process in a shared fifo, (@to_add or @to_activate).
 * 		Those processes will still be inactive, until they are read and scheduled by the select function;
 */

#include <core/type/list.h>

#include <type/containers/concurrent/shared_fifo.h>
#include "process.h"


struct sched_element {

	//The status head. Used by the active list, or by shared fifos;
	struct list_head status_head;

	//The main head. Used to reference all processes in their add order; Access is critical;
	struct list_head main_head;


	//TODO INDEX;

	//The activity state; Set if the element is active;
	bool active;

	//The priority data only concerns the priority manager, and is specified by the implementation;
	void *priority_data;

	//The process itself. Its form is not specified;
	struct prc *process;

};


struct sched_data {

	//The first process; Leads to the process list;
	struct sched_element *const main_list;

	//The first available process; Can be null. The linked list is sorted;
	struct sched_element *active_list;

	//The shared fifo, that contains processes to activate;
	struct shared_fifo to_activate;

	//The function to sort the active list;
	void (*const sort_active_list)(struct sched_data *sched, struct sched_element *new_elements);

	//If set the current process will be terminated at next @select call;
	bool termination_required;

	//If set the current process will be stopped at next @select call;
	bool stop_required;


};



//------------------------------------------------- Creation - Deletion ------------------------------------------------

//Create a scheduler in the kernel heap;
struct sched_data *sched_create(

	//The first function to execute;
	struct sched_element *init,

	//The function to insert new elements in the active list and maintaining it sorted;
	void (*const sort_active_list)(struct sched_data *sched, struct sched_element *new_elements)
);


//Delete a scheduler;
void sched_delete(struct sched_data *);


//----------------------------------------------------- Scheduling -----------------------------------------------------

/*
 * Commit changes in the scheduler internal environment. This must be done after any manipulation;
 *
 * 									!!!!
 * This function must be called at the lowest interrupt priority level;
 * 									!!!!
 *
 */

void scheduler_commit(struct sched_data *);

//Get the current process;
struct prc *sched_get(struct sched_data *);


//----------------------------------------------------- Activation -----------------------------------------------------

//Add the provided process;
void sched_add_prc(struct sched_data *, struct sched_element *);

//Resume the provided process;
void sched_resume_prc(struct sched_data *, struct sched_element *);


//---------------------------------------------------- Deactivation ----------------------------------------------------

//Set the current process in the stopped state, and return its ref;
struct sched_element *sched_stop_prc(struct sched_data *);

//Terminate the current process; Will be deleted at next select call;
void sched_terminate_prc(struct sched_data *);




#endif //TRACER_SCHEDULER_H
