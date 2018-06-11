/*
  sprocess.c Part of TRACER

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

#include "sprocess.h"

#include "scheduler.h"

#include <kernel/systick.h>

#include <kernel/kernel.h>
#include <kernel/scheduler/tasks/stask.h>


//---------------------------- Private functions ----------------------------

//The entry function for sprocesses;
void sprocess_init();

//Execution function for sprocesses;
void sprocess_function(volatile sprocess_t *volatile sprocess);

//Exit functions for sprocesses;
void sprocess_exit() { while (1); }


//---------------------------- Process initialisation ----------------------------

/*
 * sprocess_create : this function allocates a stack, and then initialises stack pointers;
 *
 *  Finally, it sets the thread's sequences_initialised as Terminated;
 */

sprocess_t *sprocess_create(size_t stack_size) {
	
	//Create the initializer;
	sprocess_t init = {
		.link = EMPTY_LINKED_ELEMENT(), 
		.stack = EMPTY_CORE_STACK(),
		.state = SPROCESS_TERMINATED,
		.task = 0,
	};
		
	//Allocate heap memory and initialise it;
	sprocess_t *sprocess = kernel_malloc_copy(sizeof(sprocess_t), &init);

	//Create a stack of the required size;
	kernel_stack_alloc(&sprocess->stack, stack_size, &sprocess_init, &sprocess_exit, sprocess);

	//Return the created process;
	return sprocess;
	
}


/*
 * sprocess_reset : resets the sprocess to its initial stack pointer,
 * and initialises the context to its beginning;
 */

void sprocess_reset(sprocess_t *sprocess) {
	
	//Reset the stack;
	kernel_stack_reset(&sprocess->stack, &sprocess_init, &sprocess_exit, sprocess);
	
	//Cache the sprocess's task;
	stask_t *task = sprocess->task;

	//If the sprocess's has a pending task, clean it;
	if (task) {
		scheduler_cleanup_task(task);
	}	

    //The sprocess has no more function to execute; Mark it terminated; 
    sprocess->state = SPROCESS_TERMINATED;

}


/*
 * sprocess_delete : frees the process task and the process;
 */

void sprocess_delete(sprocess_t *sprocess) {

	//Cleanup the process;
	sprocess_reset(sprocess);
	
	//Free the sprocess's stack;
	kernel_stack_free(&sprocess->stack);
	
	//Free the process;
	kernel_free(sprocess);

}


/*
 * sprocess_init : this function is called when a sprocess is started on the fly, 
 * 	without proper function call, due to a context switch;
 *
 *  As the sprocess pointer has been saved in the register at the thread initialisation, 
 * 	the first step is to get it back;
 *
 *  Then, we can call properly the run function;
 */

void sprocess_init() {

    //Get the thread index and run the associated thread;
    sprocess_function((volatile sprocess_t *volatile) core_get_init_arg());

}


/*
 * sprocess_function : this function is the routine executed by a process_t;
 */

void sprocess_function(volatile sprocess_t *volatile sprocess) {


    //The sprocess must be pending, otherwise, there has been an error in the scheduler.
    if ((volatile sprocess_state_t) sprocess->state != SPROCESS_PENDING) {

        //Kernel panic;
        kernel_error("sprocess.c : sprocess_function : Attempted to start a non pending process;");

    }

    //Cache the task pointer;
    volatile stask_t *volatile task = sprocess->task;

    //Execute the function, passing args;
    (*(task->function))((void *) task->args);

    //Cache the exit function;
    void (*volatile cleanup)() = task->cleanup;

    //If the function is not null, execute it;
    if (cleanup) {
        (*cleanup)();
    }

    //Mark the process_t inactive; Scheduler will be notified at context switch;
    sprocess->state = (volatile sprocess_state_t) SPROCESS_TERMINATED;

    //Require a context switch, as the task is finished;
    core_preempt_process();

	kernel_error("sprocess.c : sprocess_function : preemption invoked but failed."
						 "Might be caused by an unfinished critical section;");

	//Infinite loop;
    while (true);

}


/*
 * sprocess_execute : If we are in process mode, interrupts the current program and 
 * 	executes the provided process;
 * 
 * 	If it succeeds, the function will never return;
 */

void sprocess_execute(sprocess_t *sprocess) {

	//Compute the core process from the sprocess;
	core_process_t core_process = {
			.process_stack = &sprocess->stack,
			.activity_time = sprocess->task->activity_time,
	};

    //Execute the first process;
    core_execute_process(&core_process, (void (*)(void *volatile)) sprocess_function, sprocess);

}















