/*
  proc.c Part of TRACER

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

//The memory library required the exception stack size to be provided by the makefile;
#if !defined(KEX_STACK_SIZE)

//COmpilation fail;
#error "Error, one make parameter not provided, check your makefile"

#define KEX_STACK_SIZE 1000

#endif

//--------------------------------------------- Includes --------------------------------------------

#include "proc.h"
#include "coproc.h"

#include <kernel/async/except.h>
#include <kernel/debug/log.h>
#include <kernel/run/sched.h>
#include <kernel/clock/sysclock.h>
#include <kernel/mem/ram.h>

#include <kernel/mem/stck.h>

#include <mem.h>
#include <kernel/panic.h>



//--------------------------------------------- Vars --------------------------------------------


//A flag set if the current process must be terminated during teh next preemption;
bool prc_process_terminated = false;

//The stacks array; Will reference interrupt stacks;
static struct stck exception_stack;


//------------------------------------------------- Proc requirements --------------------------------------------------

//Initialise the stack context for future execution;
void proc_init_stack(struct stck *stack, void (*function)(), void (*end_loop)(), void *init_arg) {
	
	//Create the stack context;
	stack->sp =__proc_create_stack_context(stack->sp_reset, function, end_loop, init_arg);
	
}


/**
 * init_exception_stacks : initialises the exception stacks array;
 */

static void init_exception_stack() {
	
	//Allocate some memory for the thread's stack in the newly created heap;
	void *thread_stack = ram_alloc(KEX_STACK_SIZE);
	
	//Determine the stack's highest address;
	void *stack_reset = (void *) ((uint8_t *) thread_stack + KEX_STACK_SIZE);
	
	//Correct the stack's highest address for proper alignment;
	stack_reset = __proc_stack_align(stack_reset);
	
	//Create the stack initializer;
	struct stck cs_init = {
		
		//The stack bound, not corrected;
		.stack_limit = thread_stack,
		
		//The stack pointer, set to its reset value;
		.sp = stack_reset,
		
		//The stack reset value, corrected by the core lib;
		.sp_reset = stack_reset,
		
	};
	
	//Initialise the exception stack;
	memcpy(&exception_stack, &cs_init, sizeof(struct stck));
	
}


/**
 * kernel_enter_multitasked_mode : setups the kernel for multi-tasked mode.
 */

void proc_start_execution() {
	
	//Initialise the scheduler;
	sched_init();
	
	//Disable all interrupts;
	exceptions_disable();
	
	//Initialise the exception stacks array;
	init_exception_stack();
	
	//Initialise all coprocessors;
	coprocs_init();
	
	//Initialise the preemption;
	__prmpt_configure(KERNEL_PREEMPTION_PRIORITY);
	
	//Set the syscall handler; Same prio as preemption, won't preempt each other;
	__syscl_configure(KERNEL_PREEMPTION_PRIORITY);
	
	
	//Log;
	kernel_log_("\nKernel initialisation sequence complete. Entering thread mode ...\n");
	
	kernel_log("target: %h", exception_stack.sp);
	
	//Enter thread mode and un-privilege, provide the kernel stack for interrupt handling;
	//Interrupts will be enabled at the end of the function;
	__proc_enter_thread_mode(exception_stack.sp);
	
}


//-------------------------------------------------- Context Switching -------------------------------------------------


//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW
//TODO MONITOR STACK OVERFLOW


/**
 * kernel_get_new_stack : called by threads to get a new stack pointer, providing the current one, and the index of the
 * 	thread;
 * @param thread_id : the thread's index;
 * @param sp : the previous stack pointer;
 * @return the the new stack pointer;
 */

void *__krnl_switch_context(void *sp) {
	
	/*
	 * First call check
	 */
	
	//The first context switch must not save the stack pointer;
	static bool first_call = true;
	
	//If the sp update is allowed :
	if (first_call) {
		
		//Allow update;
		first_call = false;
		
		//Skip termination and context saving;
		goto commit;
		
	}
	
	/*
	 * Stack overflow monitoring;
	 */
	
	//Cache the current process's memory;
	struct pmem *mem = sched_get_pmem();
	
	//If a stack overflow has occurred : 
	if (sp < mem->stack.stack_limit) {
		
		//Panic.
		kernel_panic("Stack overflow");
		
	}
	
	
	/*
	 * Termination
	 */
	//TODO
	//TODO IN SYSCALL;
	
	//If the process is terminated :
	if (prc_process_terminated) {
		
		//Terminate the current process;
		sched_terminate_prc();
		
		//Clear termination flag;
		prc_process_terminated = false;
		
		//Skip context saving;
		goto commit;
		
	}
	
	
	/*
	 * Context Saving
	 */
	
	//Save coprocessors contexts;
	coprocs_save_contexts(&mem->contexts);
	
	//Save process stack pointer;
	mem->stack.sp = sp;
	
	
	/*
	 * Scheduler commit
	 */
	
	commit:
	
	//Commit changes to the scheduler;
	sched_commit();
	
	//Update the duration until next preemption;
	sysclock_set_process_duration(sched_get_req()->activity_time);
	
	
	
	/*
	 * Context loading
	 */
	
	//Update the current process memory;
	mem = sched_get_pmem();
	
	//Save coprocessors contexts;
	coprocs_load_contexts(&mem->contexts);
	
	//Return the appropriate stack pointer;
	return mem->stack.sp;
	
}
