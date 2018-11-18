/*
  run.c Part of TRACER

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

#include <stddef.h>
#include "syscall.h"

void *__run_get_init_arg();




/**
 * core_led_high : hardware code for turning on the led;
 */

void __dbg_led_high() {
	
	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;
	
	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;
	
	//Set
	*(volatile uint32_t *) 0x400FF084 = 1 << 5;
}


/**
 * core_led_low : hardware code for turning off the led;
 */

void __dbg_led_low() {
	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;
	
	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;
	
	//Clear
	*(volatile uint32_t *) 0x400FF088 = 1 << 5;
}


/**
 * core_delay : hardware code for waiting a certain number of milliseconds;
 */

void __dbg_delay_ms(uint32_t ms_counter) {
	
	while (ms_counter--) {
		//Count to;
		//for (volatile uint32_t i = 15000; i--;);
		for (volatile uint32_t i = 3000; i--;);
	}
	
}


/*
 * prc_exec : the process execution function;
 *
 * 	Executes the process function, with the process args, and returns;
 */

#include <stdbool.h>

void run_exec() {
	
	//Cache the execution data, saved in the stack;
	volatile struct prc_desc *volatile desc = __run_get_init_arg();
	
	//Execute the function, passing args;
	(*(desc->function))((void *) desc->args, (size_t) desc->args_size);
	
	
	sys_exit();
	
}


/**
 * prc_exit : the process exit function. Called automatically when prc_entry returns;
 *
 * 	Marks the process terminated and triggers the preemption;
 *
 * 	If preemption fails, an internal error has occurred, and a kernel panic is generated;
 */

void run_exit() {
	
	while (1) {}
	
}

