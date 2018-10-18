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


//#include "kinetis.h"

#include <kernel/startup.h>

#include <hard/chip/nxp/kinetis/kx_wdog.h>
#include <hard/chip/nxp/kinetis/kx_sim.h>

#include <kernel/panic.h>
#include <kernel/debug/log.h>
#include <kernel/debug/debug.h>
#include <kernel/clock/clock.h>



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


// Flash Security Setting. On Teensy 3.2, you can lock the MK20 chip to prevent
// anyone from reading your code.  You CAN still reprogram your Teensy while
// security is set, but the bootloader will be unable to respond to auto-reboot
// requests from Arduino. Pressing the program button will cause a full chip
// erase to gain access, because the bootloader chip is locked out.  Normally,
// erase occurs when uploading begins, so if you press the Program button
// accidentally, simply power cycling will run your program again.  When
// security is locked, any Program button press causes immediate full erase.
// Special care must be used with the Program button, because it must be made
// accessible to initiate reprogramming, but it must not be accidentally
// pressed when Teensy Loader is not being used to reprogram.  To set lock the
// security change this to 0xDC.  Teensy 3.0 and 3.1 do not support security lock.
#define FSEC 0xDE

// Flash Options
#define FOPT 0xF9
__attribute__ ((section(".flashconfig"), used))
const uint8_t flashconfigbytes[16] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, FSEC, FOPT, 0xFF, 0xFF
};



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
	
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	
	__krnl_init();
	
}