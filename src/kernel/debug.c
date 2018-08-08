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


/**
 * teensy35_led_high : lights the debug led and enter an empty infinite loop;
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
		debug_delay(ms_counter);
		debug_led_low();
		debug_delay(ms_counter);

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
			debug_delay(250);

			//Turn off the LED;
			debug_led_low();

			//Wait 10 ms;
			debug_delay(250);

		}

		debug_delay(2000);

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


/*
 * Morse symbols durations;
 */

#define MORSE_BIT 150
#define MORSE_DOT MORSE_BIT
#define MORSE_DASH (3 * MORSE_BIT)
#define MORSE_BIT_SPACE MORSE_BIT
#define MORSE_CHAR_SPACE (2 * MORSE_BIT)
#define MORSE_WORD_SPACE (4 * MORSE_BIT)


/**
 * morse_send_bit : sends the required bit;
 *
 * @param dash : true for a dash, false for a dot;
 */

void morse_send_bit(bool dash) {

	//Light the led;
	debug_led_high();

	//Wait the appropriate time for the symbol;
	if (dash) {
		debug_delay(MORSE_DASH);
	} else {
		debug_delay(MORSE_DOT);
	}

	//Turn off the led;
	debug_led_low();

	//Wait to separate the bit from the next one;
	debug_delay(MORSE_BIT_SPACE);

}

//A morse char has a variable length. We must store the binary code and its mask;
struct morse_char {
	uint8_t code;
	uint8_t mask;
};


/**
 * morse_send_bit : calls the bit send function for all bits of the char;
 * @param c : the morse char to send;
 */

void morse_send_char(struct morse_char c) {

	//While the code is not finished :
	while (c.mask & (uint8_t) 1) {

		//Send the morse bit;
		morse_send_bit(c.code);

		//Shift code and mask;
		c.code >>= 1;
		c.mask >>= 1;

	}

	//Delay to mark the end of the char;
	debug_delay(MORSE_CHAR_SPACE);

}


/**
 * morse_send_word : calls morse_send_char for each char of the word;
 *
 * @param sequence : the array of morse chars to send;
 * @param length : the number of chars in the array;
 */

void morse_send_word(struct morse_char *sequence, uint8_t length) {

	//While there are chars to send :
	while (length--) {

		morse_send_char(*(sequence++));

	}

	//Delay to mark the end of the char;
	debug_delay(MORSE_WORD_SPACE);


}


/*
 * The morse alphabet;
 */

#define MORSE_A {2, 3}
#define MORSE_B {2, 16}
#define MORSE_O {7, 7}
#define MORSE_S {0, 7}


/**
 * debug_sos : sends SOS in morse over the debug led;
 */

void debug_sos() {

	//Turn off the led;
	debug_led_low();

	//Cache the SOS word;
	struct morse_char sequence[3] = {MORSE_S, MORSE_O, MORSE_S};

	while (1) {
		morse_send_word(sequence, 3);

	}

}
