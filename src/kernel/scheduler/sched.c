/*
  sched.c Part of TRACER

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

#include "sched.h"

#include "sched_elmt.h"

#include <string.h>
#include <kernel/panic.h>
#include <kernel/async/interrupt.h>
#include <kernel/kinit.h>


//--------------------------------------------------- Scheduler data ---------------------------------------------------


struct sched_data {
	
	//The first process; Leads to the process list;
	struct sched_elmt *const main_list;
	
	//The first available process; Can be null. The linked list is sorted;
	struct sched_elmt *active_list;
	
	//The shared fifo, that contains processes to activate;
	struct shared_fifo to_activate;
	
	//If set the current process will be terminated at next @select call;
	bool termination_required;
	
	//If set the current process will be stopped at next @select call;
	bool stop_required;
	
};


//The first process;
struct sched_elmt first_element = {
	
	.main_head = {
		.prev = &first_element,
		.next = &first_element,
	},
	
	.status_head = {
		.prev = &first_element,
		.next = &first_element,
	},
	
	//Fist process not initialised;
	.process = 0,
	
	//First process not active;
	.active = false,
	
};


//The kernel scheduler;
struct sched_data scheduler = {
	
	//Save the first process. If it gets terminated, a kernel debug will be thrown;
	.main_list = &first_element,
	
	//The first process is available for selection;
	.active_list = &first_element,
	
	//Initialise the shared fifo empty;
	.to_activate = {
		.list = 0,
	},
	
	//No termination or stop required;
	.termination_required  = false,
	.stop_required = false,
	
};


//-------------------------------------------------- Default policy --------------------------------------------------


/*
 * A simple round robin implementation;
 */
static void round_robin_policy(struct sched_elmt **list_ref, struct sched_elmt *new_elements) {
	
	//If no elements are to insert :
	if (!new_elements) {
		
		//Do nothing, the list is not sorted;
		return;
		
	}
	
	//Simply concatenate new elements and active list;
	list_concat((struct list_head *) new_elements, (struct list_head *) *list_ref);

}


//-------------------------------------------------- Scheduler policy --------------------------------------------------

/**
 * The scheduling policy, a function in charge of inserting new scheduler elements in the list, and to maintain it
 * 	sorted;
 *
 * @param list_ref : a ref to the address where the first element of the list is stored.
 * @param new_elements : the list of new elements to insert in the list. Can be null;
 */

static scheduling_policy policy = &round_robin_policy;


/**
 * sched_set_scheduling_policy : updates the scheduling policy. If a null pointer is provided, the default round robin
 * 	policy is set;
 */

void sched_set_scheduling_policy(scheduling_policy new_policy) {
	
	//If the new policy is null :
	if (!new_policy) {
		
		//Set to the default round robin policy;
		policy = &round_robin_policy;
		
	} else {
		
		//If the new policy is not null, update the policy;
		policy = new_policy;
		
	}
	
}


//------------------------------------------------- Initialisation ------------------------------------------------

/**
 * sched_init : initialises the first process of the scheduler;
 *
 * 	The first process to execute must be provided, as well as the sort function;
 * 	
 * @param first_process : the first process to execute;
 */

void sched_init(struct prc *first_process) {

	//If the first process is null :
	if (!first_process) {

		//Error, must not happen;
		kernel_panic("sched.c : sched_create : first process is null;");

	}

	
	//Transfer the ownership of the first process;
	first_element.process = first_process;

	//The first process is active;
	first_element.active = true;

}




//------------------------------------------------- Creation - Deletion ------------------------------------------------


/**
 * sched_add_prc : adds the process in the scheduler;
 * @param sched : the scheduler in which to add the process;
 * @param element : the process;
 */

void sched_create_element(struct prc *const process) {
	
	//If the element is active :
	if (!process) {
		
		//Error, can't insert an active element;
		kernel_panic("sched_create_element : a null process was provided;");
	}
	
	//Allocate memory for the new element;
	struct sched_elmt *elmt = kmalloc(sizeof(struct sched_elmt));
	
	
	struct sched_elmt init = {
		.main_head = {
			.next = &elmt,
			.prev = &elmt,
		},
		
		.status_head = {
			.next = &elmt,
			.prev = &elmt,
		},
		
		.process = process,
		
		.active = true,
		
	};
	
	//Initialise the element;
	memcpy(elmt, &init, sizeof(struct sched_elmt));
	
	//Push the element in the add shared fifo;
	shared_fifo_push(&scheduler.to_activate, (struct list_head *) elmt);
	
	//Access to the main list is critical;
	critical_section_enter();
	
	//Concatenate the element at the end of the main list;
	elist_concat(struct sched_elmt, main_head, elmt, scheduler.main_list);
	
	//Access to the main list is critical;
	critical_section_leave();
	
	
}


/**
 * sched_delete_element :
 * 	- unlinks the element from the main lists;
 * 	- deletes the priority data;
 * 	- deletes the process;
 * 	- deletes the element;
 *
 * @param element : the element to delete;
 */

static void sched_delete_element(struct sched_elmt *element) {
	
	//Access to the process list is critical;
	critical_section_enter();
	
	//Remove the element from the main list;
	elist_remove(struct sched_elmt, main_head, element);
	
	//Access to the process list is critical;
	critical_section_leave();
	
	//Delete the process;
	prc_delete(element->process);
	
	//Delete the element;
	kfree(element);
	
}


//----------------------------------------------------- Activation -----------------------------------------------------

/**
 * sched_add_prc : activates the process belonging to the scheduler;
 * @param sched : the scheduler in which to activate the process;
 * @param element : the process;
 */

void sched_resume_prc(struct sched_elmt *const element) {
	
	//If the element is active :
	if (element->active) {
		
		//Error, can't insert an active element;
		kernel_panic("sched.c : attempted to resume an active element;");
		
	}
	
	//Mark the element active;
	element->active = true;
	
	//Push the element in the activation shared fifo;
	shared_fifo_push(&scheduler.to_activate, (struct list_head *) element);
	
}


//---------------------------------------------------- Deactivation ----------------------------------------------------

/**
 * sched_stop_prc : sets the stop flag in the provided scheduler;
 *
 * 	The current element will be stopped during next commit;
 *
 * @param sched : the scheduler whose current process must be stopped;
 * @return the element containing the process that will be stopped;
 */

struct sched_elmt *sched_stop_prc() {
	
	//Mark the stop required;
	scheduler.stop_required = true;
	
	//Return the first active element;
	return scheduler.active_list;
	
}


/**
 * sched_terminate_prc : sets the stop flag in the provided scheduler;
 *
 * 	The current element will be terminated during next commit;
 *
 * @param sched : the scheduler whose current element must be terminated;
 */

void sched_terminate_prc() {
	
	//Mark the termination required;
	scheduler.termination_required = true;
	
}


//----------------------------------------------------- Scheduling -----------------------------------------------------

/**
 * sched_active_remove_first : removes first element of the active list, and returns it;
 */

static struct sched_elmt *remove_first() {

	//Cache the first available element;
	struct sched_elmt *elmt = scheduler.active_list;

	//Cache its successor;
	struct sched_elmt *next = elmt->status_head.next;

	//If @v is the only element of the list :
	if (elmt == next) {

		//Kernel debug, total deadlock;
		kernel_panic("sched.c : total deadlock, all processes are stopped;");

	} else {
		//If @elmt has a successor :

		//Mark the successor as the first available block;
		scheduler.active_list = next;

	}

	//Remove @elmt from the active list;
	list_remove((struct list_head *)elmt);

	//Return the removed element;
	return elmt;

}


/**
 * scheduler_commit : must only be executed at the lowest possible interrupt level;
 *
 * 	- eventually terminates the current element;
 * 	- eventually stops the current element;
 * 	- eventually activates elements;
 * 	- maintains the active list sorted for integer scheduling;
 *
 * @param sched : the scheduler to commit changes to;
 */

void sched_commit() {
	
	//If the termination flag is set :
	if (scheduler.termination_required) {

		//Remove the first element of the active list;
		struct sched_elmt *removed = remove_first();

		//If we removed the first element;
		if (removed == scheduler.main_list) {

			//Kernel debug. The first process must never terminate or be terminated;
			kernel_panic("sched.c : scheduler_select : attempted to terminate the first process;");

		}

		//If the removed element can be to_delete safely, delete it;
		sched_delete_element(removed);

	} else if (scheduler.stop_required) {

		//If the current process must be stopped :

		//Remove the first element of the active list;
		struct sched_elmt *removed = remove_first();

		//Mark the element inactive;
		removed->active = false;

	}
	

	//Get the list of elements to activate; They have been marked active at their insertion in the shared fifo;
	struct sched_elmt *to_activate = (struct sched_elmt *) shared_fifo_get_all(&scheduler.to_activate);
	
	//Insert elements and sort the active list;
	(*policy)(&scheduler.active_list, to_activate);
	
}


/**
 * sched_get : return the current process;
 *
 * @param sched : the scheduler data to get the process from;
 * @return the process of the most prioritary (first) active element;
 */

struct prc *sched_get() {

	//Return the process of the first active element;
	return scheduler.active_list->process;

}

