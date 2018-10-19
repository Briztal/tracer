/*
  arm_v7m_base.c Part of TRACER

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


#include <stdint.h>

#include <stddef.h>


/*
 * Notice :
 *
 *
 *  arm v7m minimal stack frame :
 *
 *      	----------- <- pre irq / stack frame header end;
 *      XPSR
 *      PC
 *      LR
 *      R12
 *      R3
 *      R2
 *      R1
 *      R0	--------- <- hw context save PSP EXCEPTION
 *      R4
 *      R5
 *      R6
 *      R7
 *      R8
 *      R9
 *      R10
 *      R11	--------- <- sw context save
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

void *__proc_create_stack_context(void *sp_reset, void (*function)(), void (*exit_loop)(), void *arg) {
	
	//Cache the current stack pointer;
	uint32_t *sp4 = sp_reset;
	
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
	return sp4;
	
}


/*
 * proc_get_init_arg : this function will return the value of r12. If called by the process init function,
 *  it will be equal to the previous function's arg;
 */

__attribute__ ((naked)) void *__proc_get_init_arg() {
	__asm__ __volatile__(\
        "mov 	r0, 	r12 	\n\t"
		"bx	 	lr 				\n\t"
	);
}


//Determine the closest inferior address, that would respect alignment requirements;
void *__proc_stack_align(void *stack_reset) {
	
	//@core_stack_alignment is a power of 2, decrement and we obtain the mask of bits to reset;
	return (void *) ((((size_t) stack_reset) & ~((size_t) 7)));
	
}


/**
 * __proc_enter_thread_mode :
 * 	- changes the current stack from msp to psp;
 * 	- updates control to use psp;
 * 	- updates msp with the provided exception stack;
 * 	- triggers preemption;
 * 	- enables interrupts so that preemption happen;
 *
 * 	This function is pure assembly, and uses ARM calling convention. @exception_stack will be located in R0;
 *
 * @param exception_stack : the stack where exceptions should happen;
 */

__attribute__ ((naked)) void __proc_enter_thread_mode(void *exception_sp) {
	
	__asm__ __volatile__ (\

	//Disable interrupts;
	"cpsid 	i \n\t"
		
		//Save msp in R1;
		"mrs 	r1,  	msp		\n\t"
		
		//Write psp with cached value of msp;
		"msr 	psp,  	r1		\n\t"
		
		//Prepare new control value. psp used, privileged;
		"mov 	r2, 	#2 		\n\t"
		
		//Update the value of control, to use psp;
		"msr 	control, r2		\n\t"
		
		//Update the main stack pointer, so that exceptions use the exception stack;
		"msr 	msp,  r0		\n\t"
		
		//Save LR in R4, as bl will overwrite it;
		"mov 	r4,		lr		\n\t"
		
		//Set preemption pending;
		"bl 	__prmpt_trigger\n\t"
		
		//Enable all faults;
		"cpsie 	f				\n\t"
		
		//Enable all interrupts; Preemption should happen;
		"cpsie 	i				\n\t"
		
		//Return. That point should never be reached, if the preemption occurred correctly;
		"bx 	r4				\n\t"
	
	);
	
	
}
