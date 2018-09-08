/*
  proc.h Part of TRACER

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

#ifndef TRACER_CORE_H
#define TRACER_CORE_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>


/**
 * The proc stack provides the two pointers that define a descending stack;
 */

struct proc_stack {

	//The lowest bound of the stack pointer, also used to free the stack, constant;
	void *const stack_limit;

	//The current stack pointer, mutable;
	void *sp;

	//The highest value the stack pointer can take, constant;
	void *const sp_reset;

};


//The number of threads;
extern const uint8_t proc_nb_threads;

//The stack alignment, a const global power of 2 defined in the arch file;
extern const size_t proc_stack_alignment;


/**
 * proc_enter_thread_mode : this function initialises threads in a safe state. It never returns.
 * 	The preemption environment must have been initialised before;
 *
 * 	- initialises all threads in their lowest privilege state;
 * 	- updates exception stacks;
 * 	- calls the provided function, that should trigger the preemption;
 * 	- make all threads run an infinite loop;
 * 	- execute the preemption call;
 * 	- enable interrupts;
 * 	- loop;
 *
 * @param exception_stacks : processor stacks that must be used in case of interrupt;
 */

extern void proc_enter_thread_mode(struct proc_stack **exception_stacks);

//Initialise the stack for initialisation. Implemented by the proc lib;
extern void proc_init_stack(struct proc_stack *stack, void (*function)(), void (*end_loop)(), void *init_arg);

//Get the initial arg;
extern void *proc_get_init_arg();

//The context switcher; Should be set as the preemption handler for a preemption to occur;
extern void proc_context_switcher();


//Determine the closest inferior stack size, that would respect alignment requirements;
static inline size_t proc_stack_correct_size(size_t stack_size) {

	//@core_stack_alignment is a power of 2, decrement and we obtain the mask of bits to reset;
	return stack_size & ~(proc_stack_alignment - 1);

}

//Determine the closest inferior address, that would respect alignment requirements;
static inline void *proc_stack_correct_reset_ptr(void *stack_reset) {

	//@core_stack_alignment is a power of 2, decrement and we obtain the mask of bits to reset;
	//TODO REMOVE THE OFFSET;
	return (void *) ((((size_t) stack_reset) & ~(proc_stack_alignment - 1)) - 16);
}



#endif //TRACER_CORE_H
