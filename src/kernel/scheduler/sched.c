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

#include "sched.h"

#include <kernel/krnl.h>
#include <core/driver/ic.h>

/**
 * sched_delete_element :
 * 	- unlinks the element from the main lists;
 * 	- deletes the priority data;
 * 	- deletes the process;
 * 	- deletes the element;
 *
 * @param element : the element to delete;
 */

void sched_delete_element(struct sched_element *element) {

	//Access to the process list is critical;
	ic_enter_critical_section();

	//Remove the element from the main list;
	elist_remove(struct sched_element, main_head, element);

	//Access to the process list is critical;
	ic_leave_critical_section();

	//Delete the priority data;
	kfree(element->priority_data);

	//Delete the process;
	prc_delete(element->process);

	//Delete the element;
	kfree(element);

}

//------------------------------------------------- Creation - Deletion ------------------------------------------------

/**
 * sched_create : creates and initializes a scheduler in the kernel heap.
 *
 * 	The first process to execute must be provided, as well as the sort function;
 * 	
 * @param first_process : the first process to execute;
 * @param sort_function : the function that will maintain the process list sorted;
 * @return the scheduler reference;
 */

struct sched_data *sched_create(

	//The first scheduler element;
	struct sched_element *first_process,

	//The function to insert new elements in the active list and maintaining it sorted;
	void (*const sort_active_list)(struct sched_data *sched, struct sched_element *new_elements)

) {

	//If the first process is null :
	if (!first_process) {

		//Error, must not happen;
		kernel_panic("sched.c : sched_create : first process is null;");

	}

	//Link the first scheduler element to itself;
	__list_link((struct list_head *) first_process, (struct list_head *) first_process);

	struct sched_data sched_init = {

		//Save the first process. If it gets terminated, a kernel panic will be thrown;
		.main_list = first_process,

		//The first process is available for selection;
		.active_list = first_process,

		//Initialise the shared fifo empty;
		.to_activate = {
			.list = 0,
		},

		//Transmit the sort function reference;
		.sort_active_list = sort_active_list,

		//No termination or stop required;
		.termination_required  = false,
		.stop_required = false,

	};

	//Allocate, initialise and return the scheduler;
	return kialloc(sizeof(struct sched_data), &sched_init);

}


/**
 * sched_delete : deletes the scheduler struct and all its elements;
 */

void sched_delete(struct sched_data *data) {

	//Cache the first element;
	struct sched_element *const first_elmt = data->main_list, *elmt = first_elmt;

	//Delete the current block and focus on the next one :
	do {

		//Cache the next element;
		struct sched_element *const next = elmt->main_head.next;

		//Delete the current one;
		sched_delete_element(elmt);

		//Focus on the next one;
		elmt = next;

		//While we are not focused on the first;
	} while (elmt != first_elmt);

	//Delete the scheduler data structure;
	kfree(data);

}


//----------------------------------------------------- Scheduling -----------------------------------------------------


/**
 * sched_active_remove_first : removes first element of the active list, and returns it;
 */

static struct sched_element *sched_active_remove_first(struct sched_data *sched) {

	//Cache the first available element;
	struct sched_element *elmt = sched->active_list;

	//Cache its successor;
	struct sched_element *next = elmt->status_head.next;

	//If @v is the only element of the list :
	if (elmt == next) {

		//Kernel panic, total deadlock;
		kernel_panic("sched.c : total deadlock, all processes are stopped;");

	} else {
		//If @elmt has a successor :

		//Mark the successor as the first available block;
		sched->active_list = next;

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

void scheduler_commit(struct sched_data *sched) {

	//If the termination flag is set :
	if (sched->termination_required) {

		//Remove the first element of the active list;
		struct sched_element *removed = sched_active_remove_first(sched);

		//If we removed the first element;
		if (removed == sched->main_list) {

			//Kernel panic. The first process must never terminate or be terminated;
			kernel_panic("sched.c : scheduler_select : attempted to terminate the first process;");

		}

		//If the removed element can be deleted safely, delete it;
		sched_delete_element(removed);

	} else if (sched->stop_required) {

		//If the current process must be stopped :

		//Remove the first element of the active list;
		struct sched_element *removed = sched_active_remove_first(sched);

		//Mark the element inactive;
		removed->active = false;

	}

	//Get the list of elements to activate; They have been marked active at their insertion in the shared fifo;
	struct sched_element *to_activate = (struct sched_element *) shared_fifo_get_all(&sched->to_activate);

	//Insert elements and sort the active list;
	(*(sched->sort_active_list))(sched, to_activate);

}


/**
 * sched_get : return the current process;
 *
 * @param sched : the scheduler data to get the process from;
 * @return the process of the most prioritary (first) active element;
 */

struct prc *sched_get(struct sched_data *sched) {

	//Return the process of the first active element;
	return sched->active_list->process;

}


//----------------------------------------------------- Activation -----------------------------------------------------

/**
 * sched_add_prc : adds the process in the scheduler;
 * @param sched : the scheduler in which to add the process;
 * @param element : the process;
 */

void sched_add_prc(struct sched_data *const sched, struct sched_element *const element) {

	//If the element is active :
	if (element->active) {

		//Error, can't insert an active element;
		kernel_panic("sched.c : attempted to add an active element;");
	}

	//Mark the element active;
	element->active = true;

	//Push the element in the add shared fifo;
	shared_fifo_push(&sched->to_activate, (struct list_head *) element);

	//Access to the main list is critical;
	ic_enter_critical_section();

	//Concatenate the element at the end of the main list;
	elist_concat(struct sched_element, main_head, element, sched->main_list);

	//Access to the main list is critical;
	ic_leave_critical_section();


}


/**
 * sched_add_prc : activates the process belonging to the scheduler;
 * @param sched : the scheduler in which to activate the process;
 * @param element : the process;
 */

void sched_resume_prc(struct sched_data *const sched, struct sched_element *const element) {

	//If the element is active :
	if (element->active) {

		//Error, can't insert an active element;
		kernel_panic("sched.c : attempted to activate an active element;");

	}

	//Mark the element active;
	element->active = true;

	//Push the element in the activation shared fifo;
	shared_fifo_push(&sched->to_activate, (struct list_head *) element);

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

struct sched_element *sched_stop_prc(struct sched_data *const sched) {

	//Mark the stop required;
	sched->stop_required = true;

	//Return the first active element;
	return sched->active_list;

}


/**
 * sched_terminate_prc : sets the stop flag in the provided scheduler;
 *
 * 	The current element will be terminated during next commit;
 *
 * @param sched : the scheduler whose current element must be terminated;
 */

void sched_terminate_prc(struct sched_data *sched) {

	//Mark the termination required;
	sched->termination_required = true;

}


