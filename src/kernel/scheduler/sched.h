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

#include "prc.h"

#include <util/type.h>

#include <kernel/struct/shared_fifo.h>


//Pre-declare the scheduler element struct, only used in source;
struct sched_elmt;

//A constant flag, set if the scheduler is compiled and supported;
extern const bool scheduler_supported;

//---------------------------------------------------- Sched policy ----------------------------------------------------

//A scheduling policy is a function that sorts a list of scheduler elements, and inserts new elements in it;
typedef void (*scheduling_policy)(struct sched_elmt **list_ref, struct sched_elmt *new_elements);

//Update the scheduling policy;
void sched_set_scheduling_policy(scheduling_policy policy);



//------------------------------------------------- Initialisation ------------------------------------------------

//initialises the first process of the scheduler;
void sched_init(struct prc *first_process);

//----------------------------------------------------- Scheduling -----------------------------------------------------

/*
 * Commit changes in the scheduler internal environment. This must be done after any manipulation;
 *
 * 									!!!!
 * This function must be called at the lowest interrupt priority level;
 * 									!!!!
 */

void sched_commit();

//Get the current process;
struct prc *sched_get();


//----------------------------------------------------- Creation -----------------------------------------------------

//Create a scheduler element referencing the given process, and reference it in the scheduler;
void sched_create_element(struct prc *);


//----------------------------------------------------- Activation -----------------------------------------------------

//Resume the provided process;
void sched_resume_prc(struct sched_elmt *);


//---------------------------------------------------- Deactivation ----------------------------------------------------

//Set the current process in the stopped state, and return its ref;
struct sched_elmt *sched_stop_prc();

//Terminate the current process; Will be to_delete at next select call;
void sched_terminate_prc();


#endif //TRACER_SCHEDULER_H
