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



#endif