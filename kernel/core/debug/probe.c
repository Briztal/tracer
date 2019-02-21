/*
  probe.c Part of TRACER

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



#include "kernel/hard/debug.h"


/**
 * The probe adds a header, and sends the resulting PDU without modulation;
 *
 * 			x x x x x x x x
 *    0 1 0 x x x x x x x x
 *
 */





static void print_char(const char c) {

	//Turn off the led;
	__dbg_led_low();

	//Shift for the header, add the header and add the footer;
	uint16_t word = ((uint16_t)c << 3) | (uint16_t)2;

	//There are 8 + 3 = 11 bits in the message;

	//For all 11 bits :
	for (uint8_t bit = 11; bit--;) {

		//If bit is set :
		if (word & (uint16_t) 1) {

			//Logic high;
            __dbg_led_high();

		} else {

			//If bit is cleared, logic low;
            __dbg_led_low();

		}

		//Focus on next bit;
		word >>= 1;

		//Wait one millisecond time;
		debug_delay_us(250);

	}

	//Turn off the led;
    __dbg_led_low();


	//Wait three millisecond time;
	debug_delay_us(600);

}



/**
 * sw_uart_send : send the char over the serial;
 *
 * 	A low start bit, no parity bit, and two high stop bits are added;
 *
 * @param bit_time : the time of a bit;
 * @param c : the char to encode
 */

void debug_print_block(void *arg, const char *block, size_t bsize) {

	while (bsize--) {
		
		print_char(*(block++));
		
	}
	
}