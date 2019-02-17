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

//------------------------------------------------ Make parameters -----------------------------------------------

/*
 * cortex_m4f.mk must provide :
 * 	- NB_EXCEPTIONS : the number of interrupts supported by the chip.
 * 	- NVIC_RELOCATION : 1 or 0, depending on if the nvic vector table will be relocated or not;
 * 	- EXEC_LEVEL : the level of execution of the kernel;
 */


#if !defined(NB_EXCEPTIONS) || !defined(NVIC_RELOC)

#error "The number of interrupts or the nvic relocation flag was not provided. Check your makefile;"

#define NB_EXCEPTIONS 256

#define NVIC_RELOC 1

#endif


//------------------------------------------------ Includes -----------------------------------------------

#include "arm_v7m.h"

#include <kernel/hard/arch/xcpt.h>


//------------------------------------------------ Interrupt priorities ------------------------------------------------

/*
 * The ARMV7 NVIC support 8 bit priorities. The number of bits evaluated depends on the implementation, but there
 * 	are at least 3 bits evaluated, which allows the library to implement all core priority levels;
 */


//The lowest priority level;
const uint8_t __xcpt_priority_0 = 0xE0;
const uint8_t __xcpt_priority_1 = 0xC0;
const uint8_t __xcpt_priority_2 = 0xA0;
const uint8_t __xcpt_priority_3 = 0x80;
const uint8_t __xcpt_priority_4 = 0x60;
const uint8_t __xcpt_priority_5 = 0x40;
const uint8_t __xcpt_priority_6 = 0x20;
const uint8_t __xcpt_priority_7 = 0x00;

static void no_isr() {}

//----------------------------------------------- IC standard interrupts -----------------------------------------------

/**
 * ic_enable_interrupts : enables the interrupt control;
 */

void __xcpt_enable() {
	__asm__ volatile("cpsie i":: :"memory");
}


/**
 * ic_disable_interrupts : enables the interrupt control;
 */

void __xcpt_disable() {
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
	if (irq_channel >= NB_EXCEPTIONS) {
		
		//Do nothing, errors can't be triggered from here; TODO USAGE FAULT;
		return;
		
	}
	
	//If the handler is null, save the empty handler; If not, save the handler;
	__kernel_vtable[irq_channel] = (handler) ? handler : no_isr;
	
}
