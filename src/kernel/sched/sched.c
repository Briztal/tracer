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

//--------------------------------------------------- Make Parameters --------------------------------------------------

//The memory library required NB_THREADS to be provided by the makefile;
#if !defined(KFP_RAM_SIZE) || !defined(KFP_STACK_SIZE) || !defined(KFP_ACTIVITY_TIME)

//COmpilation fail;
#error "Error, one make parameter not provided, check your makefile"

#define KFP_RAM_SIZE 2048

#define KFP_STACK_SIZE 1024

#define KFP_ACTIVITY_TIME 3000

#endif


//--------------------------------------------- Includes --------------------------------------------

#include "sched.h"

#include "proc.h"

#include <string.h>
#include <kernel/panic.h>
#include <kernel/async/interrupt.h>
#include <kernel/mem/kdmem.h>
#include <kernel/debug/log.h>


//------------------------------------------------- Process execution --------------------------------------------------

/*
 * prc_exec : the process execution function. ;
 *
 * 	Executes the process function, with the process args, and returns;
 */

static void prc_exec() {
	
	kernel_log_("Entering");
	
	//Cache the execution data, saved in the stack;
	volatile struct prc_desc *volatile desc = __proc_get_init_arg();
	
	//Execute the function, passing args;
	(*(desc->function))((void *) desc->args, (size_t) desc->args_size);
	
}


/**
 * prc_exit : the process exit function. Called automatically when prc_entry returns;
 *
 * 	Marks the process terminated and triggers the preemption;
 *
 * 	If preemption fails, an internal error has occurred, and a kernel panic is generated;
 */

extern bool prc_process_terminated;

static void prc_exit() {
	
	//TODO SYSCALL;
	//TODO SYSCALL AND PREEMPTION TRIGGER;
	
	//Mark the process terminated;
	prc_process_terminated = true;
	
	//TODO SYSCALL KERNEL PREEMPT
	
	//Require a context switch, process will be to_delete;
	__prmpt_set_pending();
	
	//Panic, preemption failed;
	kernel_panic("process.c : post preempt state reached. That should never happen.");
	
}



//----------------------------------------------- First process function -----------------------------------------------

//The kernel's first process function;
extern void __kernel_first_function(void *args, size_t args_size);


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
	.req = {
		.ram_size = KFP_RAM_SIZE,
		.stack_size = KFP_STACK_SIZE,
		.activity_time = KFP_ACTIVITY_TIME,
	},
	
	.desc = {
		.function = __kernel_first_function,
		.args = 0,
		.args_size = 0,
	},
	
	//Program memory, initialised at runtime;
	.prc_mem = {0},
	
	//First process not active;
	.active = false,
	
};


//The kernel scheduler;
struct sched_data scheduler = {
	
	//Save the first process. If it gets terminated, a kernel debug.txt will be thrown;
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
	
	//Cache the current element;
	struct sched_elmt *current = *list_ref;
	
	//If no elements are to insert :
	if (new_elements) {
		
		//Simply concatenate new elements and active list;
		list_concat((struct list_head *) new_elements, (struct list_head *) current);
		
	}
	
	*list_ref = current->status_head.next;
	
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
 * sched_init_prc_mem : initialises the process memory of the provided element, and initialises its
 * 	stack, by copying its descriptor;
 *
 * @param elmt : the scheduler element to initialise;
 */

static void sched_init_prc_mem(struct sched_elmt *elmt) {
	
	//Cache the processor memory ref;
	struct prc_mem *mem = &elmt->prc_mem;
	
	//Initialise a program memory;
	prc_mem_create_heap(mem, elmt->req.ram_size);
	
	//Reset the prog mem and allocate the stack;
	prc_mem_reset(mem, elmt->req.stack_size);
	
	//The process descriptor is located in the kernel heap. We must copy it in the process heap;
	struct prc_desc *desc_copy = heap_ialloc(mem->heap, sizeof(struct prc_desc), &elmt->desc);
	
	//Initialise the stack and pass the execution environment;
	proc_init_stack(&mem->stack, &prc_exec, &prc_exit, desc_copy);
	
}


/**
 * sched_init : initialises the scheduler, by initialising its first element;
 *
 * 	The first element has all its data except its program memory initialised;
 *
 * 	This function creates its heap, its stack, and marks it activated;
 */

void sched_init() {
	
	sched_init_prc_mem(&first_element);
	
	//The first process is active;
	first_element.active = true;
	
}


//----------------------------------------------------- Operations -----------------------------------------------------

/**
 * sched_add_prc : adds the process in the scheduler;
 * @param sched : the scheduler in which to add the process;
 * @param element : the process;
 */

void sched_create_prc(struct prc_desc *desc, struct prc_req *req) {
	
	//If the element is active :
	if ((!desc) || (!req)) {
		
		//Error, can't insert an active element;
		kernel_panic("sched_create_element : a null process was provided;");
		
	}
	
	//Allocate memory for the new element;
	struct sched_elmt *elmt = kmalloc(sizeof(struct sched_elmt));
	
	struct sched_elmt init = {
		
		.status_head = {
			.next = elmt,
			.prev = elmt,
		},
		
		.main_head = {
			.next = elmt,
			.prev = elmt,
		},
		
		//Copy process structures;
		.req = *req,
		.desc = *desc,
		
		//Program memory not initialised for instance;
		.prc_mem = {0},
		
		//Process active;
		.active = true,
		
	};
	
	
	//Initialise the element;
	memcpy(elmt, &init, sizeof(struct sched_elmt));
	
	//Initialise the element's process memory and execution stack;
	sched_init_prc_mem(elmt);
	
	//Push the element in the add shared fifo;
	shared_fifo_push(&scheduler.to_activate, (struct list_head *) elmt);
	
	kernel_log_("pushed");
	
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
	prc_mem_clean(&element->prc_mem);
	
	//Delete the element;
	kfree(element);
	
}


//Set the stack pointer of one thread of the current process;
void sched_set_prc_sp(void * sp) {
	
	//Update the stack pointer of the first process;
	scheduler.active_list->prc_mem.stack.sp = sp;
	
}


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

		//Kernel debug.txt, total deadlock;
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

			//Kernel debug.txt. The first process must never terminate or be terminated;
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


//----------------------------------------------------- Query -----------------------------------------------------

//Get the current process hardware requirements;
struct prc_req *sched_get_req() {
	
	//Return the first process hardware requirements;
	return &scheduler.active_list->req;
	
}


//Get the stack pointer of one thread of the current process;
void * sched_get_sp() {
	
	//Return the first process hardware requirements;
	return scheduler.active_list->prc_mem.stack.sp;
	
}



