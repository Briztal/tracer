/*
  teensy35.h Part of TRACER

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


#ifndef TRACER_ARCH_TEENSY35_H
#define TRACER_ARCH_TEENSY35_H

#include "kernel/arch/processors/arm_cortex_m4f.h"


/*
 * --------------------------------------- ERROR HANDLING ---------------------------------------
 */

//Notify that an error occurred;
void arch_handle_error(const char *);

void arch_blink(uint16_t delay);

void arch_count(size_t count);

/*
 * --------------------------------------- PORT ---------------------------------------
 */

//We will use the kinetis PORT module;
#include <kernel/arch/peripherals/kinetis/kinetis_PORT.h>

extern struct kinetis_PORT_driver_t *PORT;


/*
 * --------------------------------------- PIT ---------------------------------------
 */

//To define PITs, we will use the kinetis PIT module;
#include <kernel/arch/peripherals/kinetis/kinetis_PIT.h>

//The teensy35 supports 4 PITs;
KINETIS_PIT_DECLARE(0);
KINETIS_PIT_DECLARE(1);
KINETIS_PIT_DECLARE(2);
KINETIS_PIT_DECLARE(3);

/*
 * --------------------------------------- UART ---------------------------------------
 */

//To define UART drivers we must include the UART driver header;
#include <kernel/arch/peripherals/kinetis/kinetis_UART.h>

/*
//The teensy35 supports 6 UARTS;
KINETIS_UART_DECLARE(0);
KINETIS_UART_DECLARE(1);
KINETIS_UART_DECLARE(2);
KINETIS_UART_DECLARE(3);
KINETIS_UART_DECLARE(4);
KINETIS_UART_DECLARE(5);
 */


//--------------------------------------- Hardware Init ---------------------------------------

void teensy35_hardware_init();

/*
//---------------------------------------- Debug -

inline void teensy35_led_high() {

	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;

	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;

	//Set
	*(volatile uint32_t *) 0x400FF084 = 1 << 5;

}

inline void teensy35_led_low() {

	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;

	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;

	//Clear
	*(volatile uint32_t *) 0x400FF088 = 1 << 5;

}


inline void teensy35_delay(uint32_t ms_counter) {

	while(ms_counter--) {
		//Count to;
		for (volatile uint32_t i = 15000; i--;);
	}

}

inline void teensy35_led_blink(uint32_t ms_counter) {

	while(true) {
		teensy35_led_high();
		teensy35_delay(ms_counter);
		teensy35_led_low();
		teensy35_delay(ms_counter);

	}

}


inline void teensy35_led_halt() {

	teensy35_led_high();

	while (true);
}

*/

#endif