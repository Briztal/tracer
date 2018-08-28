/*
  arm_cortex_m4f.h Part of TRACER

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

#ifndef CORE_ARM_CORTEXM4F
#define CORE_ARM_CORTEXM4F

#include "stdbool.h"

#include "stdint.h"

#include "stddef.h"

//-------------------------------------- Init --------------------------------------

//Initialise the core;
void core_init();


//------------------------------------- Execution -------------------------------------

//Set the process provider;
void core_set_process_provider(void (*stack_provider)(core_process_t *));

//Reset the process provider;
void core_reset_process_provider();

//Execute a process, defined by its initialised stack, its duration, and its function. An arg can be passed;
void core_execute_process(core_process_t *process, void (*function)(void *), void *arg);

//Trigger the preemption;
inline void core_preempt_process();




//---------------------------------------------------- Stack bounds ----------------------------------------------------

/**
 * core_correct_size : gets a size that fits TODO WHY THIS ?
 * @param size
 * @return
 */
static inline size_t core_correct_size(size_t size) {

	//Set the three first bits to zero to obtain a multiple of 8;
	return size & ~(size_t)7;

}


/**
 * core_get_stack_begin : determines the address of the fist byte of the provided stack.
 *
 * @param allocated_pointer : the lowest address of the block
 * @param size : the block size;
 * @return the address of the first byte of the stack;
 */

static inline void *core_get_stack_begin(void *allocated_pointer, size_t size) {

    //Count 16 bytes less to avoid stack underflow;
    return  (void *) ((uint32_t)(allocated_pointer) + ((uint32_t)size - 16));
}


/**
 * core_get_stack_end : determines the address of the last byte of the stack from the allocated block description;
 *
 *  In an arm cortex, the stack decreases. The last element is referenced by the lowest address of the block
 *
 * @param allocated_pointer : the lowest address of the allocated block;
 * @param size : the size of the allocated block;
 * @return the address of the last byte of the stack;
 */

static inline void *core_get_stack_end(void *allocated_pointer, size_t size) {
	return  allocated_pointer;
}


//------------------------------------------------- Stack management  --------------------------------------------------

//Initialise a stack for a future unstacking. An arg pointer can be passed;
void core_init_stack(core_stack_t *stack, void (*function)(), void (*end_loop)(), void *init_arg);

//Get the argument stored at stack init. Must be called at the very initial of the process function;
void *core_get_init_arg();


/*
 * ------------------------------------- Context switch -------------------------------------
 */

/*
 * core_preempt_process : sets the pendSV exception isPending flag;
 */

inline void core_preempt_process() {
    SCB_ICSR |= SCB_ICSR_PENDSVSET;
}


/*
 * ------------------------------------- Execution modes and privilleges -------------------------------------
 */

/*
 * core_in_handler_mode : this function returns true if the current code is ran from an interrupt;
 *
 *  If we are an interrupt, the 8 first bits of the System Control Block will contain the exception number.
 *
 *  If those are 0, no interupt is running yet;
 */

inline bool core_in_handler_mode() {
    return (bool) ((uint8_t) SCB_ICSR);
}





#endif
