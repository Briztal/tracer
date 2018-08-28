//
// Created by root on 8/22/18.
//


#include <core/debug.h>
#include <core/core.h>


/**
 * The probe adds a header, and sends the resulting PDU without modulation;
 *
 * 			x x x x x x x x
 *    0 1 0 x x x x x x x x
 *
 */




/**
 * sw_uart_send : send the char over the serial;
 *
 * 	A low start bit, no parity bit, and two high stop bits are added;
 *
 * @param bit_time : the time of a bit;
 * @param c : the char to encode
 */

void probe_send(char c) {

	//Turn off the led;
	core_led_low();

	//Shift for the header, add the header and add the footer;
	uint16_t word = ((uint16_t)c << 3) | (uint16_t)2;

	//There are 8 + 3 = 11 bits in the message;

	//For all 11 bits :
	for (uint8_t bit = 11; bit--;) {

		//If bit is set :
		if (word & (uint16_t) 1) {

			//Logic high;
			core_led_high();

		} else {

			//If bit is cleared, logic low;
			core_led_low();

		}

		//Focus on next bit;
		word >>= 1;

		//Wait one millisecond time;
		core_delay_us(500);

	}

	//Turn off the led;
	core_led_low();


	//Wait three millisecond time;
	core_delay_ms(7);

}


void core_log(const char *msg) {

	//Cache the current char;
	char c;

	//While the current char is not null;
	while ((c = *(msg++))) {

		//Send it;
		probe_send(c);

	}

	//Send a new line and a carriage return;
	probe_send('\n');
	probe_send('\r');

}

//Encode a char to an array of durations;
//char sw_uart_decode(uint16_t bit_time, const uint16_t *durations, size_t);
