/*
  sclk.h Part of TRACER

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
 * This file contains declarations for all khal dependant system clock functions;
 */

#ifndef TRACER_SCLK_H
#define TRACER_SCLK_H

#include <stdint.h>

//Configure the frequency and the priority of the system clock interrupt
extern void __sclk_configure(uint32_t tick_frequency, uint8_t int_prio);

//Start the system clock;
extern void __sclk_start();

//Stop the system clock;
extern void __sclk_stop();

#endif //TRACER_SCLK_H
