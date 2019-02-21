/*
  debug.c Part of TRACER

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


#include <stdbool.h>

#include <stdint.h>

#include <string.h>

#include <kernel/arch/dbg.h>


//TODO IMPLEMENTATION DEPENDANT;

#define DEBUG_TICKS_PER_MS 3000

#define DEBUG_TICKS_PER_US 3


//The debug flag, initialised to false;
bool debug_flag = false;


/**
 * debug_delay_us : hardware code for waiting a certain number of milliseconds;
 */

void debug_delay_ms(uint32_t ms_counter) {

	while (ms_counter--) {
		//Count to;
		//for (volatile uint32_t i = 15000; i--;);
		for (volatile uint32_t i = DEBUG_TICKS_PER_MS; i--;);
	}

}


/**
 * debug_delay_us : hardware code for waiting a certain number of milliseconds;
 */

void debug_delay_us(uint32_t us_counter) {

	while (us_counter--) {
		//Count to;
		for (volatile uint32_t i = DEBUG_TICKS_PER_US; i--;);
	}

}

/*
 * ----------------------------------------------------- Led Debug -----------------------------------------------------
 */

/**
 * debug_led_blink : blinks the debug led infinitely, waiting @ms_counter milliseconds between each toggle;
 *
 * @param ms_counter : the number of milliseconds between each toggle of the debug led;
 */

void debug_led_blink(uint32_t ms_counter) {

	while (1) {
		__dbg_led_high();
        debug_delay_ms(ms_counter);
        __dbg_led_low();
        debug_delay_ms(ms_counter);

	}

}


/**
 * debug_led_count : blinks the debug led @count times, waiting 250 milliseconds between each toggle, waits 2 seconds,
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
            __dbg_led_high();

			//Wait 10 ms;
            debug_delay_ms(250);

			//Turn off the LED;
            __dbg_led_low();

			//Wait 10 ms;
            debug_delay_ms(250);

		}

        debug_delay_ms(2000);

	}

}


/**
 * debug_led_cmp : blink waiting 50ms if c is not null, of 1s if c is null, infinitely;
 *
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
 * debug_led_dump : displays bits of a byte infinitely, blink fast for 1, blink slow for 0;
 *
 * @param c : the byte to dump;
 */

void debug_led_dump(const uint8_t c) {

	while(1) {

		//Cache c;
		uint8_t cc = c;

		//For each bit :
		for (uint8_t i = 8; i--;) {

			//If the bit is 1 :
			if (cc & (uint8_t) 1) {
                __dbg_led_high();
                debug_delay_ms(100);
                __dbg_led_low();
			} else {

				for (uint8_t bi = 5; bi--;) {

                    __dbg_led_high();
                    debug_delay_ms(10);
                    __dbg_led_low();
                    debug_delay_ms(10);

				}
			}
            debug_delay_ms(50);
			cc >>=1;

		}

        debug_delay_ms(100);

	}

}






