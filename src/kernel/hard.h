/*
  hard.h Part of TRACER

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
 * This file contains headers for all functions that must be implemented by hardware libraries to make the kernel
 * 	work properly.
 *
 * 	The first section contains function that are required in all modes, and the second contains functions that are
 * 	only required in scheduler mode;
 */


#ifndef TRACER_HARD_H
#define TRACER_HARD_H

#include <stdint.h>
#include <kernel/sched/proc.h>


/*
 * ---------------------------------------------------------------------------------------------------------------------
 * ----------------------------------------------- Mandatory declarations ----------------------------------------------
 * ---------------------------------------------------------------------------------------------------------------------
 */

//----------------------------------------------------- Debug ----------------------------------------------------


//Light the debug led high; Implemented by the hardware;
extern void __debug_led_high();

//Turn off the debug led; Implemented by the hardware;
extern void __debug_led_low();

//Wait for a certain number of milliseconds. Not accurate or reliable; Implemented by the hardware;
extern void __debug_delay_ms(uint32_t ms);

//Wait for a certain number of microsoconds. Not accurate or reliable; Implemented by the hardware;
extern void __debug_delay_us(uint32_t ms);

//Send a char over the debug interface, encoded by the log protocol. Implemented by the log protocol;
extern void __debug_print_char(char);

//Print the content of all registers, and the content of the stack;
extern void __debug_print_stack_trace(bool software_context_saved, uint32_t stack_depth);

//----------------------------------------------------- Vector table ---------------------------------------------------

//The kernel interrupt vector table;
extern void (*__kernel_vtable[])(void);


//------------------------------------------- General interrupt priorities -------------------------------------------

//The lowest priority level;
extern const uint8_t __ic_priority_0;
extern const uint8_t __ic_priority_1;
extern const uint8_t __ic_priority_2;
extern const uint8_t __ic_priority_3;
extern const uint8_t __ic_priority_4;
extern const uint8_t __ic_priority_5;
extern const uint8_t __ic_priority_6;
extern const uint8_t __ic_priority_7;
//The highest priority level;


//-------------------------------------------------- Exceptions -------------------------------------------------

//Enable the interrupt control;
extern void __exceptions_enable();

//Disables the interrupt control;
extern void __exceptions_disable();

//Sets the handler of the required exception;
extern void __exception_set_handler(uint16_t channel, void (*handler)());


//-------------------------------------------------- Interrupt ReQuest -------------------------------------------------

//Enables the required interrupt channel;
extern void __irq_enable(uint16_t channel);

//Disables the required interrupt channel;
extern void __irq_disable(uint16_t channel);

//Sets the required non-system interrupt in the pending state;
extern void __irq_set_pending(uint16_t channel);

//Sets the required non-system interrupt in the not-pending state;
extern void __irq_clear_pending(uint16_t channel);

//Sets the required non-system interrupt in the not-pending state;
extern void __irq_is_pending(uint16_t channel);

//Applies the provided priority to the required non-system interupt channel;
extern void __irq_set_priority(uint16_t channel, uint8_t priority);

//Returns the priority to the required non-system interupt channel;
extern uint8_t __irq_get_priority(uint16_t channel, uint8_t priority);

//Update the handler of the required irq;
extern void __irq_set_handler(uint16_t channel, void (*handler)());

//Resets the handler of the required channel to 0, isr will return immediately;
extern void __irq_reset_interrupt_handler(uint16_t channel);

//Assert if a handler is currently in execution;
extern bool __irq_in_handler_mode();


/*
 * ---------------------------------------------------------------------------------------------------------------------
 * ---------------------------------------------- Scheduler mode functions ---------------------------------------------
 * ---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------------------- Processor functions -------------------------------------------------

//Determine the closest inferior address, that would respect alignment requirements;
extern void *__proc_stack_align(void *stack_reset);

//Get the initial arg;
extern void *__proc_get_init_arg();


//The hardware library must provide a function to create the general stack context;
extern void __proc_create_stack_context(struct proc_stack *stack, void (*function)(), void (*exit_loop)(), void *arg);

//The context switcher; Should be set as the preemption handler for a preemption to occur;
extern void __proc_preemption_handler();


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

extern void __proc_enter_thread_mode(struct proc_stack *exception_stacks);


//-------------------------------------------------- Preemption functions -------------------------------------------------

extern void __preemption_set_handler(void (*)(void));

extern void __preemption_set_priority(uint8_t);

extern void __preemption_enable();

extern void __preemption_set_pending();

extern void __preemption_clear_pending();


#endif //TRACER_HARD_H
