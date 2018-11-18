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




#include <list.h>

#include "pmem.h"

#include "../../dmz/prc.h"

#include <struct/shared_fifo.h>


struct sched_elmt {
	
	//The status head. Used by the active list, or by shared fifos;
	struct list_head status_head;
	
	//The main head. Used to reference all processes in their add order; Access is critical;
	struct list_head main_head;
	
	//The process descriptor;
	struct prc_req req;
	
	//The process data;
	struct prc_desc desc;
	
	//The program memory;
	struct pmem prc_mem;
	
	//TODO ENABLE ONLY FOR DEBUG;
	//The activity state; Set if the element is active;
	bool active;
	
};


//---------------------------------------------------- Sched policy ----------------------------------------------------

//A scheduling policy is a function that sorts a list of scheduler elements, and inserts new elements in it;
typedef void (*scheduling_policy)(struct sched_elmt **list_ref, struct sched_elmt *new_elements);

//Update the scheduling policy;
void sched_set_scheduling_policy(scheduling_policy policy);


//------------------------------------------------- Initialisation ------------------------------------------------

//initialises the first process of the scheduler;
void sched_init();


//----------------------------------------------------- Operations -----------------------------------------------------

//Create a scheduler element referencing the given process, and reference it in the scheduler;
void sched_create_prc(struct prc_desc *desc, struct prc_req *req);

//Set the current process in the stopped state, and return its ref;
struct sched_elmt *sched_stop_prc();

//Resume the provided process;
void sched_resume_prc(struct sched_elmt *);

//Terminate the current process; Will be to_delete at next select call;
void sched_terminate_prc();


//----------------------------------------------------- Commit -----------------------------------------------------

/*
 * Commit changes in the scheduler internal environment. This must be done after any manipulation;
 *
 * 									!!!!
 * This function must be called at the lowest interrupt priority level;
 * 									!!!!
 */

void sched_commit();


//----------------------------------------------------- Query -----------------------------------------------------

//Get the current process;
struct prc_req *sched_get_req();

//Get the stack pointer of one thread of the current process;
struct pmem *sched_get_pmem();

#endif //TRACER_SCHEDULER_H
