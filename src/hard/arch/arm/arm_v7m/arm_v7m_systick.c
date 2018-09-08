/*
  arm_v7m_systick.c Part of TRACER

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
 * All functions from arm_v7m.h are static inline. This file makes proper definitions;
 */

#include <stdint.h>

#include "nvic.h"

#include "arm_v7m.h"


//Start the core timer;
void systimer_start() {
	armv7m_systick_select_core_clock();
	armv7m_systick_enable();
}

//Stop the core timer;
void systimer_stop() {
	armv7m_systick_disable();
}

//Enable the core interrupt;
void systimer_int_enable() {
	armv7m_systick_int_enable();
}

//Disable the core interrupt;
void systimer_int_disable() {
	armv7m_systick_int_disable();
}


//Set the core timer interrupt frequency. The core frequency must be available
void systimer_int_set_frequency(uint32_t frequency) {

	//TODO. FOR INSTANCE, ONLY THE 10MS IS LOADED;
	armv7m_systick_set_reload(8000);

}


//Set the core timer interrupt priority;
extern void systimer_int_set_priority(uint8_t priority) {

	armv7m_set_systick_priority(priority);


}


//Set the core timer interrupt handler;
void systimer_int_set_handler(void (*handler)()) {

	//TODO ERROR or log if null;

	nvic_set_exception_handler(NVIC_SYSTICK, handler);

}
