/*
  mk64fx512.h Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

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


#ifndef TRACER_MK64FX512_H
#define TRACER_MK64FX512_H

#include <stdint.h>

/*
 * --------------------------------------- PORT ---------------------------------------
 */

//We will use the K64 PORT module;
//#include <hardware/driver/kinetis/kinetis_PORT.h>

extern struct kinetis_PORT_driver_t *PORT;


/*
 * --------------------------------------- PIT ---------------------------------------
 */

//To define PITs, we will use the kinetis PIT module;
//#include <kernel/arch/driver/kinetis/kinetis_PIT.h>

extern struct kinetis_PIT_driver *PIT;


/*
 * --------------------------------------- UART ---------------------------------------
 */

//To define UART chip we must include the UART driver header;
//#include <kernel/arch/driver/kinetis/kinetis_UART.h>

extern struct kinetis_UART_driver_t *UART0;


//---------------------------------------------------- Hardware Init ---------------------------------------------------

//The code entry point;
void _entry_point();


//-------------------------------------------------------- Debug -------------------------------------------------------

/**
 * chip_led_high : hardware code for turning on the led;
 */

static inline void chip_led_high() {

	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;

	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;

	//Set
	*(volatile uint32_t *) 0x400FF084 = 1 << 5;

}


/**
 * chip_led_low : hardware code for turning off the led;
 */

static inline void chip_led_low() {

	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;

	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;

	//Clear
	*(volatile uint32_t *) 0x400FF088 = 1 << 5;

}


/**
 * chip_delay : hardware code for waiting a certain number of milliseconds;
 */

static inline void chip_delay(uint32_t ms_counter) {

	while (ms_counter--) {
		//Count to;
		for (volatile uint32_t i = 15000; i--;);
	}

}





#endif