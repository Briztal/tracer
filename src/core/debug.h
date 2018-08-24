/*
  debug.h Part of TRACER

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

#ifndef TRACER_DEBUG_H
#define TRACER_DEBUG_H

#include <stdint.h>

#include <stddef.h>



//------------------------------------------------------ LED debug -----------------------------------------------------

/*
 * This library contains led based debug functions.
 *
 * 	They only use hardware functions, and can be used anywhere in the code, without restrictions on the kernel state;
 */



//Light the debug led high;
extern void debug_led_high();

//Turn off the debug led;
extern void debug_led_low();

//Wait a certain number of milliseconds;
extern void debug_delay_ms(uint32_t ms);

//Wait a certain number of milliseconds;
extern void debug_delay_us(uint32_t us);

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

//Execute an BREAK error sequence indefinitely;
void debug_break();

//Execute an SOS error sequence indefinitely;
void debug_sos();

//Convert a string in morse symbols and send it;
void debug_echo(const char *log);

//Halt and send a message;
void debug_error(const char *msg);


#endif //TRACER_DEBUG_H
