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

#include "stdint.h"

#include "stdbool.h"

#include "stddef.h"

#include "kinetis.h"

/*
 * -------------------------------------- Types --------------------------------------
 */

//The stack element type;
//typedef uint32_t stack_element_t;

//The stack pointer type;
//typedef stack_element_t *stack_ptr_t;

//The process stack type;
typedef struct {
	
	//The current stack pointer;
    void *stack_pointer;

	//The beginning of the stack;
    void *stack_begin;
	
	//The end of the stack;
    void *stack_end;

} stack_t;

//The empty stack initializer;
#define EMPTY_STACK() (stack_t) {.stack_pointer = 0, .stack_begin = 0, .stack_end = 0,}


//TODO STACK MANAGEMENT;


//-------------------------------------- Init --------------------------------------

//Initialise the core;
void core_init();


//------------------------------------- Stack bounds -------------------------------------

//Correct a size to get a 4 bytes multiple;
inline size_t core_correct_size(size_t size);

//Get the stack begin from the allocated region;
inline void *core_get_stack_begin(void *allocated_pointer, size_t size);

//Get the stack end from the allocated region;
inline void *core_get_stack_end(void *allocated_pointer, size_t size);


//------------------------------------- Stack management  -------------------------------------

//Initialise a stack for a future unstacking. An arg pointer can be passed
void *core_init_stack(void *sp, void (*function)(), void (*end_loop)(), void *init_arg);

//Get the argument stored at stack init. Must be called at the very beginning of the process function;
void *core_get_init_arg();


//------------------------------------- Execution -------------------------------------

//Set the stack provider;
void core_set_stack_provider(stack_t *(*stack_provider)(stack_t *));

//Reset the stack provider;
void core_reset_stack_provider();

//Execute a process, defined by its initialised stack;
void core_execute_process(stack_t *stack, void (*volatile function)(void *), void *volatile arg);

//Trigger the preemption;
inline void core_preempt_process();


//------------------------------------- Execution modes and privilleges -------------------------------------

//Returns true if the current code runs in thread mode
inline bool core_in_handler_mode();


//------------------------------------- Interrupts ------------------------------------- 

/*
 * Priority levels : these are the standard interrupt priorities for drivers;
 *
 * Cortex-M4: 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240
 */


//The standard priority for status interrupt;
#define DRIVER_STARUS_INTERRUPT_PRIORITY 32

//The standard priority for error interrupt;
#define DRIVER_ERROR_INTERRUPT_PRIORITY 16


//Enable all interrupts;
inline void core_enable_interrupts();

//Disable all interrupts;
inline void core_disable_interrupts();


//Enable the specified interrupt;
inline void core_IC_enable(uint8_t interrupt_index);

//Disable the specified interrupt
inline void core_IC_disable(uint8_t interrupt_index); 

//Mark the interrupt pending. Will be executed asap;
inline void core_IC_set_pending(uint8_t interrupt_index);
  
//Mark the interrupt as not pending;
inline void core_IC_clear_pending(uint8_t interrupt_index);

//Set the priority of the required interrupt to the povided;
inline void core_IC_set_priority(uint8_t interrupt_index, uint8_t priority); 

//Set the handler of the required interrupt;
inline void core_IC_set_handler(uint8_t interrupt_index, void (*handler)());


/*
 * ------------------------------------- Stack bounds -------------------------------------
 */

/*
 * core_correct_size : corrects the size to get a multiple of 4.
 */

inline size_t core_correct_size(size_t size) {
	
	//Set the two first bytes to zero to obtain a multiple of 4;
	return size & ~(size_t)3;
	
}


/*
 * core_get_stack_begin : determines the stacks beginning case from the allocated pointer and the size;
 *
 *  In an arm cortex, the stack decreases, and the stack pointer points to the last pushed element;
 */

inline void *core_get_stack_begin(void *allocated_pointer, size_t size) {
	return ((uint8_t *)(allocated_pointer)) + size;
}


/*
 * core_get_stack_end : determines the stacks end case from the allocated pointer and the size;
 *
 *  In an arm cortex, the stack decreases, and the stack pointer points to the last pushed element;
 */

inline void *core_get_stack_end(void *allocated_pointer, size_t size) {
	return  allocated_pointer;
}



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


/*
 * ------------------------------------- Interrupts -------------------------------------
 */

/*
 * core_enable_interrupts : injects inline assembly code to disable all interrupts. 
 * 	Fault handling is still enabled;
 */

inline void core_enable_interrupts() {
	__asm__ __volatile__("cpsie i");
}


/*
 * core_disable_interrupts : injects inline assembly code to disable all interrupts. 
 * 	Fault handling is still enabled;
 */

inline void core_disable_interrupts() {
	__asm__ __volatile__("cpsid i");
}


/*
 * core_IC_enable : enables the specified interrupt;
 */

inline void core_IC_enable(uint8_t interrupt_index) {
    NVIC_ENABLE_IRQ(interrupt_index);
}


/*
 * core_IC_disable : disables the specified interrupt;
 */

inline void core_IC_disable(uint8_t interrupt_index) {
    NVIC_DISABLE_IRQ(interrupt_index);
}


/*
 * core_IC_set_pending : marks the interrupt as pending. Will be executed asap;
 */

inline void core_IC_set_pending(uint8_t interrupt_index) {
    NVIC_SET_PENDING(interrupt_index);
}


/*
 * core_IC_clear_pending :  marks the interrupt as not pending;
 */

inline void core_IC_clear_pending(uint8_t interrupt_index) {
    NVIC_CLEAR_PENDING(interrupt_index);
}


/*
 * core_IC_set_priority : sets the priority of the required interrupt to the povided;
 */

inline void core_IC_set_priority(uint8_t interrupt_index, uint8_t priority) {
    NVIC_SET_PRIORITY(interrupt_index, priority);
}


/*
 * core_IC_set_handler : sets the handler of the required interrupt;
 */

inline void core_IC_set_handler(uint8_t interrupt_index, void (*handler)()) {
    _VectorsRam[16 + (interrupt_index)] = handler;
}


#endif
