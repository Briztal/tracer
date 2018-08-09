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
 * --------------------------------------- PORT ---------------------------------------
 */

//We will use the kinetis PORT module;
#include <kernel/arch/drivers/kinetis/kinetis_PORT.h>

extern struct kinetis_PORT_driver_t *PORT;


/*
 * --------------------------------------- PIT ---------------------------------------
 */

//To define PITs, we will use the kinetis PIT module;
#include <kernel/arch/drivers/kinetis/kinetis_PIT.h>

extern struct kinetis_PIT_driver *PIT;


/*
 * --------------------------------------- UART ---------------------------------------
 */

//To define UART drivers we must include the UART driver header;
#include <kernel/arch/drivers/kinetis/kinetis_UART.h>

extern struct kinetis_UART_driver_t *UART0;


//---------------------------------------------------- Hardware Init ---------------------------------------------------

void teensy35_hardware_init();


//-------------------------------------------------------- Debug -------------------------------------------------------

#define DEBUG_LINK(func) teensy35##_##func

//Light the debug led high;
void teensy35_led_high();

//Turn off the debug led;
void teensy35_led_low();

//Wait a certain number of milliseconds;
void teensy35_delay(uint32_t ms_counter);


#endif