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

//Declare A, B, C, D and E ports;
KINETIS_PORT_DECLARE(A);
KINETIS_PORT_DECLARE(B);
KINETIS_PORT_DECLARE(C);
KINETIS_PORT_DECLARE(D);
KINETIS_PORT_DECLARE(E);


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
#include <kernel/drivers/UART.h>

//The teensy35 supports 6 UARTS;
KINETIS_UART_DECLARE(0);
KINETIS_UART_DECLARE(1);
KINETIS_UART_DECLARE(2);
KINETIS_UART_DECLARE(3);
KINETIS_UART_DECLARE(4);
KINETIS_UART_DECLARE(5);

#endif