/*
  mk64fx512.c Part of TRACER

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


#include <kernel/startup.h>

#include "kx_wdog.h"
#include "kx_sim.h"


//------------------------------------------------------- Debug --------------------------------------------------------

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


/**
 * core_delay : hardware code for waiting a certain number of milliseconds;
 */

void __dbg_delay_us(uint32_t us_counter) {
	
	while (us_counter--) {
		//Count to;
		for (volatile uint32_t i = 3; i--;);
	}
	
}



void __proc_init(void) {
	
	/*
	 * First, disable the watchdog;
	 */
	
	//Disable the watchdog;
	k64_wdog_disable();
	
	//Enable gating for all ports;
	sim_enable_PORTA_clock_gating();
	sim_enable_PORTB_clock_gating();
	sim_enable_PORTC_clock_gating();
	sim_enable_PORTD_clock_gating();
	sim_enable_PORTE_clock_gating();
	
	startup_initialise_globals();
	
	__krnl_init();
	
}