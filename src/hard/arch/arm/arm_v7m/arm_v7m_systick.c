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


#include <stdint.h>
#include <kernel/clock/clock.h>
#include <kernel/mod/auto_mod.h>
#include <kernel/clock/sysclock.h>
#include <kernel/panic.h>
#include <kernel/async/interrupt.h>

#include "arm_v7m.h"


//Start the core timer;
static void t_start() {
	armv7m_systick_select_core_clock();
	armv7m_systick_enable();
}


//Stop the core timer;
void t_stop() {
	armv7m_systick_disable();
}


//Enable the core interrupt;
static void t_int_enable() {
	armv7m_systick_int_enable();
}


//Disable the core interrupt;
static void t_int_disable() {
	armv7m_systick_int_disable();
}


//Set the core timer interrupt frequency. The core frequency must be available
static void t_int_set_frequency(uint32_t frequency) {

	//Get the core clock value;
	uint32_t core_freq = clock_get("core");
	
	//If the core frequency is null :
	if (!core_freq) {
		
		//Panic. The core clock is not registered;
		kernel_panic("systick_set_frequency : core frequency not registered;");
		
	}
	
	//Compute the reload;
	uint32_t reload = core_freq / frequency;
	
	//Update the reload;
	armv7m_systick_set_reload(reload);

}


//Set the core timer interrupt priority;
static void t_int_set_priority(uint8_t priority) {

	armv7m_set_systick_priority(priority);
	
}


//Set the core timer interrupt handler;
static void t_int_set_handler(void (*handler)()) {

	exception_set_handler(NVIC_SYSTICK, handler);

}

static struct sys_timer stick = {
	
	.int_enable = &t_int_enable,
	.int_disable = &t_int_disable,
	
	.int_set_handler = &t_int_set_handler,
	.int_set_priority = &t_int_set_priority,
	.int_set_frequency = &t_int_set_frequency,
	
	.start = &t_start,
	.stop = &t_stop,
	
};


static bool systick_init() {
	
	//Register the system timer;
	sysclock_init(&stick);
	
	//Complete;
	return true;
	
}


KERNEL_EMBED_MODULE(PROC_MODULE, systick, &systick_init);