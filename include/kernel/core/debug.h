/*
  debug.h Part of TRACER

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

#ifndef TRACER_DEBUG_H
#define TRACER_DEBUG_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include <kernel/khal/dbg.h>



//hardware code for waiting a certain number of milliseconds;
void debug_delay_ms(uint32_t ms_counter);

//debug_delay_us : hardware code for waiting a certain number of milliseconds;
void debug_delay_us(uint32_t us_counter);


/*
 * ----------------------------------------------------- Led Debug -----------------------------------------------------
 */

//Blink the debug.txt led indefinitely, waiting @ms_counter between each toggle;
void debug_led_blink(uint32_t ms_counter);

//Blink @count times, wait, and re-iterate;
void debug_led_count(size_t count);

//Blink at high frequency if c != 0, or at low frequency if c == 0;
void debug_led_cmp(size_t c);

//Display bits of a byte, blink fast for 1, blink slow for 0;
void debug_led_dump(uint8_t c);


/*
 * ----------------------------------------------------- print -----------------------------------------------------
 */

void debug_print_block(void *arg, const char *block, size_t bsize);


/*
 * ----------------------------------------------------- Log vars -----------------------------------------------------
 */

//A flag, to use for debug.txt purposes;
extern bool debug_flag;


#endif //TRACER_DEBUG_H
