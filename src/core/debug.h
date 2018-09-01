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



//------------------------------------------------------ LED debug -----------------------------------------------------

/*
 * This library contains led based debug functions.
 *
 * 	They only use hardware functions, and can be used anywhere in the code, without restrictions on the kernel state;
 */


//Light the debug led and stop;
void debug_led_halt();

//Blink the debug led indefinitely, waiting @ms_counter between each toggle;
void debug_led_blink(uint32_t ms_counter);

//Blink @count times, wait, and re-iterate;
void debug_led_count(size_t count);

//Blink at high frequency if c != 0, or at low frequency if c == 0;
void debug_led_cmp(size_t c);

//Display bits of a byte, blink fast for 1, blink slow for 0;
void debug_led_dump(uint8_t c);

//A flag, to use for debug purposes;
extern bool debug_flag;

//Break + msg : sends break concatenated with the message, and waits for one second;
#define DEBUG_BREAK(i) {core_log("break"#i);core_delay_ms(300);}

//Break_wait + msg + time : sends break concatenated with the message, and waits for one second;
#define DEBUG_BREAK_WAIT(i, time) {core_log("break"#i); core_delay_ms(time);}


#endif //TRACER_DEBUG_H
