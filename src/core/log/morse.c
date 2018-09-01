/*
  morse.c Part of TRACER

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


#include "core/debug.h"

#include <stdbool.h>


/**
 * A morse char has a variable length. We must store the binary code and its mask;
 */

struct morse_char {
	uint8_t code;
	uint8_t mask;
};


/*
 * The morse alphabet;
 */

#define MORSE_A {2, 3}
#define MORSE_B {1, 15}
#define MORSE_C {5, 15}
#define MORSE_D {1, 7}
#define MORSE_E {0, 1}
#define MORSE_F {4, 15}
#define MORSE_G {3, 7}
#define MORSE_H {0, 15}
#define MORSE_I {0, 3}
#define MORSE_J {14, 15}
#define MORSE_K {5, 7}
#define MORSE_L {2, 15}
#define MORSE_M {3, 3}
#define MORSE_N {1, 3}
#define MORSE_O {7, 7}
#define MORSE_P {6, 15}
#define MORSE_Q {11, 15}
#define MORSE_R {2, 7}
#define MORSE_S {0, 7}
#define MORSE_T {1, 1}
#define MORSE_U {4, 7}
#define MORSE_V {8, 15}
#define MORSE_W {6, 7}
#define MORSE_X {9, 15}
#define MORSE_Y {13, 15}
#define MORSE_Z {3, 15}

#define MORSE_0 {31, 31}
#define MORSE_1 {30, 31}
#define MORSE_2 {28, 31}
#define MORSE_3 {24, 31}
#define MORSE_4 {16, 31}
#define MORSE_5 {0, 31}
#define MORSE_6 {1, 31}
#define MORSE_7 {3, 31}
#define MORSE_8 {7, 31}
#define MORSE_9 {15, 31}


static const struct morse_char morse_alphabet[26] = {
	MORSE_A, MORSE_B, MORSE_C, MORSE_D, MORSE_E,
	MORSE_F, MORSE_G, MORSE_H, MORSE_I, MORSE_J,
	MORSE_K, MORSE_L, MORSE_M, MORSE_N, MORSE_O,
	MORSE_P, MORSE_Q, MORSE_R, MORSE_S, MORSE_T,
	MORSE_U, MORSE_V, MORSE_W, MORSE_X, MORSE_Y,
	MORSE_Z,

};


static const struct morse_char morse_numbers[10] = {
	MORSE_0, MORSE_1, MORSE_2, MORSE_3, MORSE_4,
	MORSE_5, MORSE_6, MORSE_7, MORSE_8, MORSE_9,
};


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
		debug_delay_ms(MORSE_DASH);
	} else {
		debug_delay_ms(MORSE_DOT);
	}

	//Turn off the led;
	debug_led_low();

	//Wait to separate the bit from the next one;
	debug_delay_ms(MORSE_BIT_SPACE);

}


/**
 * morse_send_char : calls the bit send function for all bits of the char;
 * @param c : the morse char to send;
 */

void morse_send_char(struct morse_char c) {

	//While the code is not finished :
	while (c.mask & (uint8_t) 1) {

		//Send the morse bit;
		morse_send_bit(c.code & (uint8_t) 0x01);

		//Shift code and mask;
		c.code >>= 1;
		c.mask >>= 1;

	}

	//Delay to mark the end of the char;
	debug_delay_ms(MORSE_CHAR_SPACE);

}


/**
 * morse_word_space : waits the time of a word spacing;
 */

inline void morse_word_space() {

	//Delay to mark the end of the char;
	debug_delay_ms(MORSE_WORD_SPACE);

}



/**
 * morse_send_if : sends the char if it belongs to [@lower; @higher].
 *
 * 	If so, the char has its offset from @lower removed, and the related morse char is selected
 * 	from @chars;
 *
 * @param lower : the lower bound;
 * @param higher : the higher bound;
 * @param chars : the morse alphabet to use;
 * @param c : the char to examine;
 * @return true if the char was sent;
 */

bool morse_send_if(const char lower, const char higher, const struct morse_char *const symbols, const char c) {

	//If the char is between @lower and @higher:
	if ((c <= higher) && (c >= lower)) {

		//Determine the associated morse symbol and send;
		morse_send_char(symbols[c - lower]);

		//Assert as a char has been sent;
		return true;
	}

	//Fail;
	return false;

}


//TODO
void debug_echo(const char *log) {

	//Turn off the led;
	debug_led_low();

	//Declare the current char var;
	char c;

	//To avoid sending multiple spaces that would consume time, a flag is declared;
	bool space = false;

	//For each char :
	do {

		//Cache the current char and increment the char pointer;
		c = *(log++);

		//If the current char is a space :
		if (c == ' ') {

			if (!space) {

				//Wait the time of a word space;
				morse_word_space();

				//Set the space flag to avoid multiple spaces;
				space = true;
			}

		}

		//If the char is sent :
		if (
			morse_send_if('a', 'z', morse_alphabet, c) ||
			morse_send_if('A', 'Z', morse_alphabet, c) ||
			morse_send_if('0', '9', morse_numbers, c)
			)

		{

			//Clear the space flag;
			space = false;

		}


	} while (c);

}
