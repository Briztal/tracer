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

#include <stdbool.h>

#include <stdint.h>

#include <kernel/mem/stack_data.h>

/*
 * ---------------------------------------------------------------------------------------------------------------------
 * ---------------------------------------------------- Kernel Hooks ---------------------------------------------------
 * ---------------------------------------------------------------------------------------------------------------------
 */

/*
 * The hardware library has access to the following kernel functions, that it can hook to ISRs to allow the kernel
 * 	to work properly;
 */

//The kernel initialisation function;
extern void __krnl_init();


//Place the kernel in a safe state, call the fault analyser, and recover, if possible. If not, panic;
extern void __krnl_handle_fault(uint32_t type);

//The kernel context switcher; received a stack_data pointer and returns the new one;
extern void *__krnl_switch_context(void *sp);

//The kernel syscall handler;
extern uint32_t __krnl_syscall_handler(uint32_t syscall_id, uint32_t arg0, uint32_t arg1, uint32_t arg2);

//The kernel tick function; Should be called by the system timer ISR;
extern void __krnl_tick();


/*
 * ---------------------------------------------------------------------------------------------------------------------
 * ----------------------------------------- Mandatory Hardware implementation -----------------------------------------
 * ---------------------------------------------------------------------------------------------------------------------
 */

//--------------------------------------------------- Initialisation ---------------------------------------------------

//The processor initialisation function;
extern void __proc_init();

//The chip initialisation function; Called by __proc_init;
extern void __chip_init();



//----------------------------------------------------- Debug ----------------------------------------------------

//Light the debug led high; Implemented by the hardware;
extern void __dbg_led_high();

//Turn off the debug led; Implemented by the hardware;
extern void __dbg_led_low();

//Wait for a certain number of milliseconds. Not accurate or reliable; Implemented by the hardware;
extern void __dbg_delay_ms(uint32_t ms);

//Wait for a certain number of microsoconds. Not accurate or reliable; Implemented by the hardware;
extern void __dbg_delay_us(uint32_t ms);

//Send a char over the debug interface, encoded by the log protocol. Implemented by the log protocol;
extern void __dbg_print_char(char);

//Print the content of all registers, and the content of the stack_data;
extern void __dbg_print_stack_trace(uint32_t *psp, bool software_context_saved, uint32_t stack_depth);

//----------------------------------------------------- Vector table ---------------------------------------------------

//The kernel interrupt vector table;
extern void (*__kernel_vtable[])(void);


//------------------------------------------- General exception priorities -------------------------------------------

//The lowest priority level;
extern const uint8_t __xcpt_priority_0;
extern const uint8_t __xcpt_priority_1;
extern const uint8_t __xcpt_priority_2;
extern const uint8_t __xcpt_priority_3;
extern const uint8_t __xcpt_priority_4;
extern const uint8_t __xcpt_priority_5;
extern const uint8_t __xcpt_priority_6;
extern const uint8_t __xcpt_priority_7;
//The highest priority level;


//-------------------------------------------------- Exceptions -------------------------------------------------

//Enable the interrupt control;
extern void __xcpt_enable();

//Disables the interrupt control;
extern void __xcpt_disable();


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


//-------------------------------------------------- System clock -------------------------------------------------

//Configure the frequency and the priority of the system clock interrupt
extern void __sclk_configure(uint32_t tick_frequency, uint8_t int_prio);

//Start the system clock;
extern void __sclk_start();

//Stop the system clock;
extern void __sclk_stop();


//------------------------------------------------------ Link vars -----------------------------------------------------


//The available RAM bounds;
extern uint8_t __ram_min, __ram_max;

//Load and virtual min and max addresses of data section (resp) in FLASH, RAM and RAM;
extern uint8_t __data_lma_min, __data_vma_min, __data_vma_max;

//virtual min and max addresses of bss section in RAM;
extern uint8_t __bss_vma_min,  __bss_vma_max;


//The min and max address of proc modules structs in FLASH;
extern const uint8_t __prmod_min, __prmod_max;

//The min and max address of system modules structs in FLASH;
extern const uint8_t __smod_min, __smod_max;

//The min and max address of periph modules structs in FLASH;
extern const uint8_t __pemod_min, __pemod_max;

//The min and max address of kernel modules structs in FLASH;
extern const uint8_t __kmod_min, __kmod_max;

//The min and max address of user modules structs in FLASH;
extern const uint8_t __umod_min, __umod_max;


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

//The hardware library must provide a function to create the general stack_data context;
extern void __proc_create_stack_context(struct stack_data *stack, void (*function)(), void (*exit_loop)(), void *arg);


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

extern void __proc_enter_thread_mode(struct stack_data *exception_stacks);


//------------------------------------------------ Preemption functions ------------------------------------------------

//Configure the priority of the preemption interruption, and enable it;
extern void __prmpt_configure(uint8_t int_prio);

//Set the preemption pending;
extern void __prmpt_set_pending();


//------------------------------------------------------- Syscall ------------------------------------------------------

//Set the priority of the syscall exception and enable it;
extern void __syscl_configure(uint8_t priority);

//Call the kernel;
extern uint32_t __syscall_trigger(uint32_t syscall_id, uint32_t arg0, uint32_t arg1, uint32_t arg2);


#endif //TRACER_HARD_H
