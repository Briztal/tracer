/*
  prcs.c Part of TRACER

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

#include "process.h"

#include "scheduler.h"

#include <kernel/systick.h>

#include <kernel/syscall.h>
#include <kernel/scheduler/tasks/stask.h>
#include <kernel/krnl.h>
#include <string.h>



//---------------------------- Private functions ----------------------------

//The entry function for prcses;
void prcs_init();

//Execution function for prcses;
void prcs_function(volatile struct prcs *volatile prcs);

//Exit functions for prcses;
void prcs_exit() { while (1); }


//---------------------------- Process initialisation ----------------------------

/*
 * prcs_create : this function allocates a stack, and then initialises stack pointers;
 *
 *  Finally, it sets the thread's sequences_initialised as Terminated;
 */

/**
 * prcs_create : allocates ram data for the process and creates and initialises its reference struct in the
 * 	kernel heap:
 *
 *
 * @param ram_size : the ram size allocated to the process;
 * @param nb_stacks : the number of stacks available for the process;
 * @param stack_size : the size of each stack;
 * @return an initialised prcs struct;
 */

struct prcs *prcs_create(size_t ram_size, uint8_t nb_stacks, size_t stack_size) {

	//Allocate memory for the process;
	struct prcs *prcs = kmalloc(sizeof(struct prcs));

	//Create the program memory, referenced in the kernel heap;
	struct prog_mem *mem = prog_mem_create(ram_size, nb_stacks, stack_size);

	//Create the initializer;
	struct prcs prcs_init = {

		//Initialise the head;
		.head = {
			.prev = prcs,
			.next = prcs,
		},

		//Transfer the ownership of the memory;
		.prog_mem = mem,

		//The process has no task, it is terminated;
		.state = PRCS_TERMINATED,

		//No task;
		.task = 0,

	};

	//Initialise the process struct;
	memcpy(prcs, &prcs_init, sizeof(struct prcs));

	//Create a stack of the required size;
	kernel_stack_alloc(&sprocess->stack, stack_size, &sprocess_init, &sprocess_exit, sprocess);


	//Return the created process;
	return prcs;
	
}


/*
 * prcs_reset : resets the prcs to its initial stack pointer,
 * and initialises the context to its initial;
 */
/**
 * prcs_reset : resets the process referenced by the struct;
 *
 * @param prcs : the process reference;
 */
void prcs_reset(struct prcs *prcs) {

	//Reset the heap;
	
	//Reset the stack;
	kernel_stack_reset(&prcs->stack, &prcs_init, &prcs_exit, prcs);
	
	//Cache the prcs's task;
	stask_t *task = prcs->task;

	//If the prcs's has a pending task, clean it;
	if (task) {
		scheduler_cleanup_task(task);
	}	

    //The prcs has no more function to execute; Mark it terminated; 
    prcs->state = PRCS_TERMINATED;

}


/*
 * prcs_delete : frees the process task and the process;
 */

void prcs_delete(struct prcs *prcs) {

	//Cleanup the process;
	prcs_reset(prcs);
	
	//Free the prcs's stack;
	kernel_stack_free(&prcs->stack);
	
	//Free the process;
	kernel_free(prcs);

}


/*
 * prcs_init : this function is called when a prcs is started on the fly, 
 * 	without proper function call, due to a context switch;
 *
 *  As the prcs pointer has been saved in the register at the thread initialisation, 
 * 	the first step is to get it back;
 *
 *  Then, we can call properly the run function;
 */

void prcs_init() {

    //Get the thread index and run the associated thread;
    prcs_function((volatile struct prcs *volatile) core_get_init_arg());

}


/*
 * prcs_function : this function is the routine executed by a process_t;
 */

void prcs_function(volatile struct prcs *volatile prcs) {



    //The prcs must be pending, otherwise, there has been an error in the scheduler.
    if ((volatile prcs_state_t) prcs->state != PRCS_PENDING) {

        //Kernel panic;
        kernel_error("prcs.c : prcs_function : Attempted to start a non pending process;");

    }

    //Cache the task pointer;
    volatile stask_t *volatile task = prcs->task;

    //Execute the function, passing args;
    (*(task->function))((void *) task->args);

    //Cache the exit function;
    void (*volatile cleanup)() = task->cleanup;

    //If the function is not null, execute it;
    if (cleanup) {
        (*cleanup)();
    }

    //Mark the process_t inactive; Scheduler will be notified at context switch;
    prcs->state = (volatile prcs_state_t) PRCS_TERMINATION_REQUIRED;

    //Require a context switch, as the task is finished;
    core_preempt_process();


    kernel_error("prcs.c : prcs_function : preemption invoked but failed."
						 "Might be caused by an unfinished critical section;");

	//Infinite loop;
    while (true);

}


/*
 * prcs_execute : If we are in process mode, interrupts the current program and 
 * 	executes the provided process;
 * 
 * 	If it succeeds, the function will never return;
 */

void prcs_execute(struct prcs *prcs) {

	//Compute the core process from the prcs;
	core_process_t core_process = {
			.process_stack = &prcs->stack,
			.activity_time = prcs->task->activity_time,
	};

    //Execute the first process;
	core_execute_process_DO_NOT_USE_THIS(&core_process, (void (*)(void *volatile)) prcs_function, prcs);

}















