/*
  arm_v7m_adv.c Part of TRACER

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

/*
 * This source file contains code related to kernel advanced mode. It contains code for preemption, syscalls, and
 * 	context switching.
 */



//------------------------------------------------ Includes -----------------------------------------------

#include <stdint.h>
#include <kernel/async/interrupt.h>
#include <kernel/panic.h>
#include <kernel/debug/log.h>
#include "arm_v7m.h"


//----------------------------------------------------- Preemption -----------------------------------------------------

//Set the handler of the preemption exception;
void __preemption_set_handler(void (*handler)(void)) {
	exception_set_handler(NVIC_PENDSV, handler);
	
	
}

//Set the priority of the preemption exception;
void __preemption_set_priority(uint8_t priority) {
	armv7m_set_pendsv_priority(priority);
}

//Enable the preemption exception;
void __preemption_enable() {
	//PendSV is always enabled;
}

//Set the preemption exception pending;
void __preemption_set_pending() {
	armv7m_set_pendsv_pending();
}

//Set the preemption exception not pending;
void __preemption_clear_pending() {
	armv7m_clr_pendsv_pending();
}



//----------------------------------------------------- Syscall -----------------------------------------------------

//Set the handler of the syscall exception;
void __syscall_set_handler(void (*handler)(void)) {
	exception_set_handler(NVIC_SVC, handler);
}

//Set the priority of the syscall exception;
void __syscall_set_priority(uint8_t priority) {
	armv7m_set_svcall_priority(priority);
}

//Enable the syscall exception;
void __syscall_enable() {
	//Always enabled;
}

//Trigger the syscall;
void __syscall_trigger() {
	__asm__ __volatile ("");//TODO SVC ??? NOT WORKING...
}


//-------------------------------------------------- Stack management  -------------------------------------------------

/*
 * Notice :
 *
 *
 *  arm v7m minimal stack frame :
 *
 *      ----------- <- pre irq / stack frame header end;
 *      XPSR
 *      PC
 *      LR
 *      R12
 *      R3
 *      R2
 *      R1
 *      R0--------- <- post irq
 */

/*
 * proc_init_stack : this function initialises the unstacking environment, so that the given function will
 *  be executed at context switch time;
 *
 *  It starts by caching the process_t stack pointer, and stacks the process functions pointers, and the PSR.
 *
 *  Then, it saves the process index in R12 (next word);
 *
 *  Finally, it leaves space for empty stack frame and saves the stack pointer;
 */

void __proc_create_stack_context(struct proc_stack *stack, void (*function)(), void (*exit_loop)(), void *arg) {
	
	//Cache the current stack pointer;
	uint32_t *sp4 = stack->sp;
	
	//Store the PSR. Contains the execution mode; //TODO DOC
	*(sp4 - 1) = 0x01000000;
	
	//Store the function in PC cache
	*(sp4 - 2) = (uint32_t) (function);
	
	//Store the return function in LR;
	*(sp4 - 3) = (uint32_t) (exit_loop);
	
	//Store the arg in R12 cache;
	*(sp4 - 4) = (uint32_t) (arg);
	
	//Update the stack pointer; Hits the future R4 reload address;
	sp4 -= 16;
	
	//Return the stack pointer;
	stack->sp = sp4;
	
}


/*
 * proc_get_init_arg : this function will return the value of r12. If called by the process init function,
 *  it will be equal to the previous function's arg;
 */

void *__proc_get_init_arg() {
	uint32_t arg = 0;
	__asm__ __volatile__("mov %0, r12": "=r" (arg):);
	return (void *) arg;
}


//Determine the closest inferior address, that would respect alignment requirements;
void *__proc_stack_align(void *stack_reset) {
	
	//@core_stack_alignment is a power of 2, decrement and we obtain the mask of bits to reset;
	return (void *) ((((size_t) stack_reset) & ~((size_t) 7)));
	
}


//---------------------------------------------------- Threads setup ---------------------------------------------------

/**
 * core_enter_thread_mode : this function initialises threads in a safe state. It never returns.
 * 	The preemption environment must have been initialised before;
 *
 * 	- initialises all threads in their lowest privilege state;
 * 	- updates exception stacks;
 * 	- triggers the preemption;
 * 	- make all threads run an infinite loop;
 *
 * @param exception_stacks : stacks that must be used in case of interrupt;
 * @param preemption_call : the function to execute in the executing thread when all other threads are idle;
 */

void __proc_enter_thread_mode(struct proc_stack *exception_stacks) {
	
	//Disable all interrupts;
	exceptions_disable();
	
	//A static var will insure that the function is called at most once;
	static bool entered = false;
	
	//If the flag has already been set, the function is called for the second time.
	if (entered) {
		
		//Error, must not happen;
		kernel_panic("proc_enter_thread_mode : called a second time;");
		
	}
	
	//Mark the function called;
	entered = true;
	
	//If we are in handler mode, ignore the request;
	if (irq_in_handler_mode()) {//TODO
		
		//Error, this function can't be executed in handler mode;
		kernel_panic("proc_enter_thread_mode : in handler mode;");
		
		//Never reached;
		return;
		
	}
	
	static volatile void *volatile msp;
	static volatile void *volatile psp;
	
	//Save the exception stack value in msp;
	msp = exception_stacks[0].sp;
	
	//Save the current main stack pointer in psp's cache;
	__asm__ __volatile__ ("mrs %0, msp" : "=r" (psp):);
	
	//Update the main stack pointer so that exceptions occur in the exception stack;
	__asm__ __volatile__ ("msr msp, %0\n\t"::"r" (msp):"memory");
	
	//Update the process stack pointer;
	__asm__ __volatile__ ("msr psp, %0\n\t"::"r" (psp):"memory");
	
	//Update the control register to use PSP;//TODO UNPRIVILLEGE
	__asm__ __volatile__(\
        "mov r4, #2 \n\t"\
        "msr control, r4\n\t":: :"r4", "cc", "memory"
	);
	
	
	//Execute the preemption call;
	__preemption_set_pending();
	
	
	//Enable interrupts;
	__asm__ __volatile__("cpsie i");
	
	kernel_panic("NO_PREEMPTION");
	
	//Wait eternally for the preemption to be triggered;
	while (1);
	
}


//----------------------------------------------------- Preemption -----------------------------------------------------

//The kernel context switcher;
extern void *proc_switch_context(void *sp);


/*
 * The context switcher :
 *
 *  - saves the current thread context;
 *  - calls the scheduler;
 *  - loads the new thread context;
 */

void __proc_preemption_handler() {
	
	//A static variable will contain the temporary psp_cache that we transmit to the stack provider;
	static void *volatile psp_cache;
	
	
	//As R0 is already saved in memory, we can use it;
	__asm__ __volatile__ (\
        "cpsid i \n\t"
		"mrs r0, psp \n\t"
		"sub r0, #32\n\t"
		"stmia r0, {r4 - r11}\n\t"
		"mov %0, r0 \n\t"
		"ISB\n\t"
		"cpsie i\n\t"
	: "=r" (psp_cache) : : "cc", "memory"
	);
	
	
	//__debug_print_stack_trace(true, 24);
	
	//Provide the old stack and get a new one; There is only one thread, with the index 0;
	psp_cache = proc_switch_context(psp_cache);
	
	//Unstack the context from the new stack; R0 will be updated at interrupt prempt, we can use it;
	//__asm__ __volatile__ ("msr psp, %0 \n\t"::"r" (psp_cache):"cc", "memory");
	//__debug_print_stack_trace(true, 24);
	
	
	//Unstack the context from the new stack; R0 will be updated at interrupt prempt, we can use it;
	__asm__ __volatile__ (\
        "cpsid i \n\t"
		"mov r0, %0 \n\t"
		"ldmia r0!, {r4 - r11} \n\t"
		"msr psp, r0 \n\t"
		"ISB\n\t"
		"cpsie i\n\t"
	::"r" (psp_cache):"cc", "memory"
	);
	
	
}
