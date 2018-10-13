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

/*
 * This source file contains the base code make the kernel work properly in loop mode.
 * 	Code related to preemption, syscalls (advanced mode) can be found in arm_v7m_adv.c
 * 	All other components (FPU, MPU, etc...) included in modules;
 */


//------------------------------------------------ Make parameters -----------------------------------------------

/*
 * Makefile must provide :
 * 	- NB_INTERRUPTS : the number of interrupts supported by the chip.
 */

#if !defined(NB_INTERRUPTS)

#error "The number of interrupts was not provided. Check your makefile;"

#define NB_INTERRUPTS 256

#endif


//------------------------------------------------ Includes -----------------------------------------------

#include <stdint.h>
#include <kernel/async/interrupt.h>
#include <kernel/panic.h>
#include <kernel/async/fault.h>
#include "arm_v7m.h"


//---------------------------------------------------- ARM_V7M Init ----------------------------------------------------


extern void __proc_init();

static void arm_v7m_init() {
	
	//Enable all faults;
	
	armv7m_enable_bus_fault();
	armv7m_enable_usage_fault();
	armv7m_enable_mem_fault();
	
	armv7m_set_mem_fault_priority(0);
	armv7m_set_bus_fault_priority(0);
	armv7m_set_usage_fault_priority(0);
	
	//Call the sched init function;
	__proc_init();
	
	
}


//------------------------------------------------------- Faults -------------------------------------------------------

static void arm_v7m_hard_fault_handler() {
	kernel_handle_fault(0);
}


static void arm_v7m_bus_fault_handler() {
	kernel_handle_fault(1);
}


static void arm_v7m_mem_fault_handler() {
	kernel_handle_fault(2);
}



static void arm_v7m_usg_fault_handler() {
	kernel_handle_fault(2);
}


//------------------------------------------------- kernel vector table ------------------------------------------------

/*
 * Some data is required to start the processor properly. Namely :
 * 	- The initial stack pointer, provided by the linker;
 * 	- The first function to execute, defined in another piece of code;
 */


extern uint32_t _ram_highest;


//Define an empty ISR handler
static void no_isr() {};

//Define the kernel vtable, and align it on 512 bytes.
void (*__kernel_vtable[NB_INTERRUPTS])(void) __attribute__ ((aligned (512))) = {
	
	//0 : not assigned;
	&no_isr,
	
	//1 : reset; Effectively used when NVIC is relocated;
	&arm_v7m_init,
	
	//2 : NMI. Not supported for instance;
	&no_isr,
	
	//3 : HardFault.
	&arm_v7m_hard_fault_handler,
	
	//4 : MemManage fault;
	&arm_v7m_mem_fault_handler,
	
	//5 : BusFault.
	&arm_v7m_bus_fault_handler,
	
	//6 : UsageFault;
	&arm_v7m_usg_fault_handler,
	
	//7 : Reserved;
	&no_isr,
	
	//8 : Reserved;
	&no_isr,
	
	//9 : Reserved;
	&no_isr,
	
	//10 : Reserved;
	&no_isr,
	
	//11 : SVCall. Support provided by a module;
	&no_isr,
	
	//12 : Reserved;
	&no_isr,
	
	//13 : Reserved;
	&no_isr,
	
	//14 : PendSV. Support provided by a module;
	&no_isr,
	
	//15 : SysTick. Support provided by a module;
	&no_isr,
	
	//All interrupts not handled;
	[16 ... NB_INTERRUPTS - 1] = &no_isr,
	
};

//-------------------------------------------------- Interrupt priorities --------------------------------------------------

/*
 * The ARMV7 NVIC support 8 bit priorities. The number of bits evaluated depends on the implementation, but there
 * 	are at least 3 bits evaluated, which allows the library to implement all core priority levels;
 */

//The lowest priority level;
const uint8_t __ic_priority_0 = 0xE0;
const uint8_t __ic_priority_1 = 0xC0;
const uint8_t __ic_priority_2 = 0xA0;
const uint8_t __ic_priority_3 = 0x80;
const uint8_t __ic_priority_4 = 0x60;
const uint8_t __ic_priority_5 = 0x40;
const uint8_t __ic_priority_6 = 0x20;
const uint8_t __ic_priority_7 = 0x00;


//----------------------------------------------- IC standard interrupts -----------------------------------------------

/**
 * ic_enable_interrupts : enables the interrupt control;
 */

void __exceptions_enable() {
	__asm__ volatile("cpsie i":: :"memory");
}


/**
 * ic_disable_interrupts : enables the interrupt control;
 */

void __exceptions_disable() {
	__asm__ volatile("cpsid i":: :"memory");
}


/**
 * irq_enable : enables the required irq channel;
 * @param channel : the channel to enable;
 */

void __irq_enable(uint16_t channel) {
	armv7m_nvic_enable_interrupt(channel);
}

/**
 * irq_disable : disables the required irq channel;
 * @param channel : the channel to disable;
 */

void __irq_disable(uint16_t channel) {
	armv7m_nvic_disable_interrupt(channel);
}


/**
 * irq_set_pending : sets the required irq in the pending state;
 * @param channel : the channel to set in the pending state;
 */

void __irq_set_pending(uint16_t channel) {
	armv7m_nvic_set_interrupt_pending(channel);
}


/**
 * irq_clear_pending : sets the required irq in the not-pending state;
 * @param channel : the channel to set in the not-pending state;
 */

void __irq_clear_pending(uint16_t channel) {
	armv7m_nvic_clear_interrupt_pending(channel);
}

/**
 * irq_is_pending : sets the required irq in the not-pending state;
 * @param channel : the channel to set in the not-pending state;
 */

void __irq_is_pending(uint16_t channel) {
	armv7m_nvic_clear_interrupt_pending(channel);
}


/**
 * irq_set_priority : applies the provided priority to the required non-system interupt channel;
 *
 * @param channel : the channel to apply the priority;
 * @param priority : the priority to apply
 */

void __irq_set_priority(uint16_t channel, uint8_t priority) {
	armv7m_nvic_set_priority(channel, priority);
}

/**
 * irq_get_priority : applies the provided priority to the required non-system interupt channel;
 *
 * @param channel : the channel to apply the priority;
 * @param priority : the priority to apply
 */

uint8_t __irq_get_priority(uint16_t channel, uint8_t priority) {
	return armv7m_nvic_get_priority(channel);
}


/**
 * irq_reset_interrupt_handler : resets the handler of the required channel to 0, isr will return immediately;
 * @param channel : the interrupt channel to update;
 */

void __irq_reset_interrupt_handler(uint16_t channel) {
	__irq_set_handler(channel, &no_isr);
}


/**
 * irq_in_handler_mode :
 *
 * @return true is a handler is curently in execution;
 */
bool __irq_in_handler_mode() {
	return (bool) armv7m_get_exception_number();
}


/**
 * irq_set_handler : updates the required irq handler;
 *
 * @param non_system_channel : the channel to update. must be inferior to 240;
 *
 * @param handler : the handler. Can be null if interrupt must be disabled;
 */

void __irq_set_handler(uint16_t irq_channel, void ( *handler)()) {
	
	//Translate to exception channel;
	irq_channel += 16;
	
	//If the channel is invalid :
	if (irq_channel >= NB_INTERRUPTS) {
		
		//Do nothing, errors can't be triggered from here; TODO USAGE FAULT;
		return;
		
	}
	
	//If the handler is null, save the empty handler; If not, save the handler;
	__kernel_vtable[irq_channel] = (handler) ? handler : no_isr;
	
}


void __exception_set_handler(uint16_t channel, void (*handler)()) {
	
	//If the channel is invalid :
	if (channel >= 16) {
		
		//Do nothing;
		return;
		
	}
	
	//If the handler is null, save the empty handler; If not, save the handler;
	__kernel_vtable[channel] = (handler) ? handler : no_isr;
	
}


//-------------------------------------------------- NVIC static base --------------------------------------------------


//If the vtable must not be generated (relocated after, smaller executable) :
#ifdef NO_VTABLE

/*
 * In order to start the processor properly, we define here the vector table, that is hard-copied in the firmware
 * 	as it, at the link section .vector. This section can be found in the link script, and starts at address 0;
 */

void *vtable[NB_INTERRUPTS] __attribute__ ((section(".vectors"))) = {
	
	//0 : Initial SP Value; In ARM Architecture, the stack pointer decreases;
	&_ram_highest,
	
	//1 : Reset : call the program's entry point;
	&__arm_v7m_init,
	
	//In order to avoid writing 254 times the function name, we will use macros that will write it for us;
#define channel(i) &no_isr,

//Redirect all isrs to the empty one;
#include "nvic_channel_list.h"

#undef channel
	
	//Adding another "&no_isr" will cause a compiler warning "excess array initializer. Try it, it is funny !
	
};


//If the vtable must be generated
#else //NOVTABLE


/**
 * isr_generic_handler : in order to support handler update, all functions of the in-flash vector table will
 * 	lead to this function, that executes the appropriate function;
 *
 * @param i : the interrupt channel. 0 to 240;
 */

static void isr_generic_flash_handler(uint8_t i) {
	
	//Execute the handler;
	(*__kernel_vtable[i])();
	
}


/*
 * Generate an ISR for each interrupt channel; Done using XMacro;
 */

//The handler link : a function that calls the handler link with a specific value;
#define channel(i) static void isr_##i() {isr_generic_flash_handler(i);}

//Define all isrs;
#include "nvic_channel_list.h"

#undef channel


/*
 * In order to start the processor properly, we define here the vector table, that is hard-copied in the firmware
 * 	as it, at the link section .vector. This section can be found in the link script, and starts at address 0;
 */

void *vtable[NB_INTERRUPTS] __attribute__ ((section(".vectors"))) = {
	
	//0 : Initial SP Value; In ARM Architecture, the stack pointer decreases;
	&_ram_highest,
	
	//1 : Reset : call the program's entry point;
	&arm_v7m_init,
	
	
	//2->255 : empty handler (240 times, 240 = 3 * 8 * 10);
	
	//In order to avoid writing 254 times the function name, we will use macros that will write it for us;
#define channel(i) &isr_##i,

//Redirect all isrs to the empty one;
#include "nvic_channel_list.h"

#undef channel
	
	//Adding another "&empty_isr" will cause a compiler warning "excess array initializer. Try it, it is funny !
	
};


#endif //NOVTABLE
