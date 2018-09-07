/*
  prc.c Part of TRACER

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

#include "prc.h"

#include <kernel/krnl.h>

#include <string.h>
#include <kernel/core.h>
#include <kernel/panic.h>


bool prc_process_terminated = false;

//---------------------------- Private functions ----------------------------


//The process execution function. Retrieves the execution data, executes the process function and returns;
static void prc_exec();

//The process prempt function. Called when prc_entry returns; Calls the kernel for termination;
static void prc_exit();


//---------------------------- Process initialisation ----------------------------

/**
 * prc_exec_env : contains all data the process need to run;
 */

struct prc_exec_env {

	//The process function;
	void (*function)(void *args);

	//The function's arguments;
	const void *args;

};


/*
 * prc_create : this function allocates a stack, and then initialises stack pointers;
 *
 *  Finally, it sets the thread's sequences_initialised as Terminated;
 */

/**
 * prc_create : allocates ram data for the process and creates and initialises its reference struct in the
 * 	kernel heap:
 *
 *
 * @param ram_size : the ram size allocated to the process;
 * @param nb_stacks : the number of stacks available for the process;
 * @param stack_size : the size of each stack;
 * @return an initialised prc struct;
 */

struct prc *prc_create(const struct prc_desc *const desc) {

	//Allocate memory for the process;
	struct prc *prc = kmalloc(sizeof(struct prc));

	//Create the program memory, referenced in the kernel heap;
	struct prog_mem *mem = prog_mem_create(desc->ram_size, false);

	//Create stacks;
	prog_mem_create_stacks(mem, desc->nb_threads, desc->stack_size);

	//Create the initializer;
	struct prc prc_init = {
		
		//Copy the process descriptor; Args will be updated after;
		.desc = *desc,

		//Transfer the ownership of the memory;
		.prog_mem = mem,

	};

	//Copy args in the process heap and update the reference;
	prc_init.desc.args = heap_ialloc(mem->heap, desc->args_size, desc->args);

	//Initialise the process struct;
	memcpy(prc, &prc_init, sizeof(struct prc));


	//Create the process execution data initializer;
	struct prc_exec_env exec_data_init = {
		.function = desc->function,
		.args = desc->args,
	};

	//Create and initialize the execution data structure in the process heap;
	struct prc_exec_env *exec_data = heap_ialloc(mem->heap, sizeof(struct prc_exec_env), &exec_data_init);

	//TODO INITIALISE ALL STACKS; WATCH ARGUMENTS CAREFULY;

	//Initialise the stack and pass the execution environment;
	core_init_stack(prc->prog_mem->stacks[0], &prc_exec, &prc_exit, exec_data);

	//Return the created process;
	return prc;
	
}


/*
 * prc_delete : frees the process task and the process;
 */

void prc_delete(struct prc *prc) {

	//Cleanup the program memory;
	prog_mem_delete_from_kernel_heap(prc->prog_mem);

	//Free the process;
	kfree(prc);

}


//------------------------------------------------------- Runtime ------------------------------------------------------

/*
 * prc_exec : the process execution function. ;
 *
 * 	Executes the process function, with the process args, and returns;
 */

static void prc_exec() {

	//Cache the execution data, saved in the stack;
	volatile struct prc_exec_env *volatile exec_data = core_get_init_arg();

	//Execute the function, passing args;
	(*(exec_data->function))((void *) exec_data->args);

}


/**
 * prc_exit : the process exit function. Called automatically when prc_entry returns;
 *
 * 	Marks the process terminated and triggers the preemption;
 *
 * 	If preemption fails, an internal error has occurred, and a kernel panic is generated;
 */

static void prc_exit() {

	//TODO SYSCALL;
	//TODO SYSCALL AND PREEMPTION TRIGGER;

	//Mark the process terminated;
	prc_process_terminated = true;

	//TODO SYSCALL KERNEL PREEMPT

	//Require a context switch, process will be to_delete;
	core_preemption_trigger();

	//Panic, preemption failed;
	kernel_panic("process.c : post preempt state reached. That should never happen.");

}
















