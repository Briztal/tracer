/*
  debug.c Part of TRACER

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

#include "debug.h"

#include <stdbool.h>


/**
 * debug_led_halt : lights the debug led and enter an empty infinite loop;
 */

void debug_led_halt() {

	debug_led_high();

	while (1);
}


/**
 * debug_led_blink : blinks the debug led, waiting @ms_counter milliseconds between each toggle;
 * @param ms_counter : the number of milliseconds between each toggle of the debug led;
 */

void debug_led_blink(uint32_t ms_counter) {

	while (1) {
		debug_led_high();
		debug_delay_ms(ms_counter);
		debug_led_low();
		debug_delay_ms(ms_counter);

	}

}


/**
 * debugled_count : blinks the debug led @count times, waiting 250 milliseconds between each toggle, waits 2 seconds,
 * 	and re-iterate;
 *
 * @param count : the number of times the led must be blinked at each iteration;
 */

void debug_led_count(size_t count) {

	//Indefinately :
	while (1) {

		//For each blink to do :
		for (size_t c = count; c--;) {

			//Turn on the LED;
			debug_led_high();

			//Wait 10 ms;
			debug_delay_ms(250);

			//Turn off the LED;
			debug_led_low();

			//Wait 10 ms;
			debug_delay_ms(250);

		}

		debug_delay_ms(2000);

	}

}


/**
 * debug_led_cmp : blink waiting 50ms if c is not null, of 1s if c is null;
 * @param c : the value to adjust the blink delay on;
 */

void debug_led_cmp(size_t c) {

	if (c) {
		debug_led_blink(50);
	} else {
		debug_led_blink(1000);
	}

}

/**
 * debug_break : sends BREAK in morse over the debug led;
 */

void debug_break() {

	while(true) {

		//Send BREAK;
		debug_echo("break");

		//Wait for one second;
		debug_delay_ms(1000);

	}

}



/**
 * debug_sos : sends SOS in morse over the debug led;
 */

void debug_sos() {


	while(true) {

		//Send BREAK;
		debug_echo("sos");

		//Wait for one second;
		debug_delay_ms(1000);

	}

}


/**
 * debug_sos : sends SOS in morse over the debug led;
 */

void debug_error(const char *msg) {

	while(true) {

		//Send BREAK;
		debug_echo(msg);

		//Wait for one second;
		debug_delay_ms(1000);

	}

}

