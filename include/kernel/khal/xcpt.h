/*
  xcpt.h Part of TRACER

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
 * This file contains declarations for all khal dependant exceptions related functions;
 */


#ifndef TRACER_XCPT_H
#define TRACER_XCPT_H

#include <stdbool.h>

#include <stdint.h>


/*------------------------------------------- General exception priorities -------------------------------------------

/*The lowest priority level;*/
extern const uint8_t __xcpt_priority_0;
extern const uint8_t __xcpt_priority_1;
extern const uint8_t __xcpt_priority_2;
extern const uint8_t __xcpt_priority_3;
extern const uint8_t __xcpt_priority_4;
extern const uint8_t __xcpt_priority_5;
extern const uint8_t __xcpt_priority_6;
extern const uint8_t __xcpt_priority_7;
/*The highest priority level;*/


/*-------------------------------------------------- Exceptions -------------------------------------------------

/*Initialise the exceptions manager;*/
void __xcpt_init();

/*Enable the exceptions handling;*/
extern void __xcpt_enable();

/*Disables the exceptions handling;*/
extern void __xcpt_disable();


/*-------------------------------------------------- Interrupt ReQuest -------------------------------------------------

/*Enables the required interrupt channel;*/
extern void __irq_enable(uint16_t channel);

/*Disables the required interrupt channel;*/
extern void __irq_disable(uint16_t channel);

/*Sets the required non-system interrupt in the pending state;*/
extern void __irq_set_pending(uint16_t channel);

/*Sets the required non-system interrupt in the not-pending state;*/
extern void __irq_clear_pending(uint16_t channel);

/*Sets the required non-system interrupt in the not-pending state;*/
extern void __irq_is_pending(uint16_t channel);

/*Applies the provided priority to the required non-system interupt channel;*/
extern void __irq_set_priority(uint16_t channel, uint8_t priority);

/*Returns the priority to the required non-system interupt channel;*/
extern uint8_t __irq_get_priority(uint16_t channel, uint8_t priority);

/*Update the handler of the required irq;*/
extern void __irq_set_handler(uint16_t channel, void (*handler)());

/*Resets the handler of the required channel to 0, isr will return immediately;*/
extern void __irq_reset_interrupt_handler(uint16_t channel);

/*Assert if a handler is currently in execution;*/
extern bool __irq_in_handler_mode();

#endif /*TRACER_XCPT_H*/
