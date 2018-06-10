//
// Created by root on 4/2/18.
//

#include "sprocess.h"

#include "scheduler.h"

#include <kernel/systick.h>

#include <kernel/kernel.h>


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
		.stack = EMPTY_STACK(), 
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
	task_t *task = sprocess->task;

	//If the sprocess's has a pending task, clean it;
	if (task) {
		//TODO;
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

	
/*
    //The sprocess must be pending, otherwise, there has been an error in the scheduler.
    if ((volatile sprocess_state_t) sprocess->state != SPROCESS_PENDING) {

        //Kernel panic;
        //TODO KERNEL PANIC;

    }
*/

    //Cache the task pointer;
    volatile task_t *volatile task = sprocess->task;
	
	//kernel_halt(30);
		
	/*		
	if (sprocess->task == &empty_task) {

        	kernel_halt(50);

    } else {

        kernel_halt(2000);

    }	
*/

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

    //Infinite loop;
    while (true) {

        //TODO ERROR, FUNCTION CALLED FROM INTERRUPT OR CRITICAL SECTION LEAVE FORGOT IN THE CODE;

    }

}


/*
 * sprocess_execute : If we are in process mode, interrupts the current program and 
 * 	executes the provided process;
 * 
 * 	If it succeeds, the function will never return;
 */

void sprocess_execute(sprocess_t *sprocess) {

    //Execute the first process; //TODO CLEANUP;
    core_execute_process(&sprocess->stack, sprocess_function, sprocess);

}















