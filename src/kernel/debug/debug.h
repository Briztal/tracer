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



/*
 * ----------------------------------------------------- Led Debug -----------------------------------------------------
 */

//Light the debug led high; Implemented by the hardware;
extern void debug_led_high();

//Turn off the debug led; Implemented by the hardware;
extern void debug_led_low();


//Blink the debug led indefinitely, waiting @ms_counter between each toggle;
void debug_led_blink(uint32_t ms_counter);

//Blink @count times, wait, and re-iterate;
void debug_led_count(size_t count);

//Blink at high frequency if c != 0, or at low frequency if c == 0;
void debug_led_cmp(size_t c);

//Display bits of a byte, blink fast for 1, blink slow for 0;
void debug_led_dump(uint8_t c);


/*
 * ---------------------------------------------------- Tempo debug ----------------------------------------------------
 */

//Wait for a certain number of milliseconds. Not accurate or reliable; Implemented by the hardware;
extern void debug_delay_ms(uint32_t ms);

//Wait for a certain number of microsoconds. Not accurate or reliable; Implemented by the hardware;
extern void debug_delay_us(uint32_t ms);


/*
 * ----------------------------------------------------- Log Debug -----------------------------------------------------
 */

/*
 * ----------------------------------------------------- Log Debug -----------------------------------------------------
 */


//Send a char over the debug interface, encoded by the log protocol. Implemented by the log protocol;
void debug_print_char(char);


//Send a string over the debug interface, encoded by the log protocol. Implemented by the log protocol;
void debug_log(const char *msg);

//Convert an integer to its string expression in the required base, and send it;
void debug_log_base(uint32_t integer, uint8_t base);

//Convert an integer to its binary string expression and send it;
#define debug_log_bin(i) {debug_log_base(i, 2);}

//Convert an integer to its octal string expression and send it;
#define debug_log_oct(i) {debug_log_base(i, 8);}

//Convert an integer to its decimal string expression and send it;
#define debug_log_int(i) {debug_log_base(i, 10);}

//Convert an integer to its hexadecimal string expression and send it;
#define debug_log_hex(i) {debug_log_base(i, 16);}

//Break + msg : sends break concatenated with the message, and waits for one second;
#define DEBUG_BREAK(i) {core_log("break"#i);core_delay_ms(300);}

//Break_wait + msg + time : sends break concatenated with the message, and waits for one second;
#define DEBUG_BREAK_WAIT(i, time) {core_log("break"#i); core_delay_ms(time);}


/*
 * ----------------------------------------------------- Log vars -----------------------------------------------------
 */

//A flag, to use for debug purposes;
extern bool debug_flag;


#endif //TRACER_DEBUG_H
