//
// Created by root on 8/22/18.
//


#include <core/debug.h>


/**
 * sw_uart_get_bit_time : determines the bit time for a given BaudRate;
 *
 * 	Both values are linked by the following equation :
 *
 * 		BT = 10^6 / BR
 *
 * 	1 < bit_time < 65535  =>  152 < BaudRate < 1000000
 *
 * 	No check is made on eventual values, if the baudrate is not in the upper bounds, an uncaught overflow will happen;
 *
 * 	If a null baudrate is provided, 65535 will be returned;
 *
 * @param baudrate
 * @return
 */

uint16_t sw_uart_get_bit_time(uint32_t baudrate) {

	//If the BaudRate is null (0 symb per second), return the maximal bit time;
	if (!baudrate) {
		return (uint16_t) -1;
	}

	//Apply the formula BT = 10^6 / BR
	return (uint16_t) ((uint32_t) 1000000 / baudrate);

}


/**
 * sw_uart_send : send the char over the serial;
 *
 * 	A low start bit, no parity bit, and two high stop bits are added;
 *
 * @param bit_time : the time of a bit;
 * @param c : the char to encode
 */

void sw_uart_send(const uint16_t bit_time, const char c) {

	//Add the low stop bit and two high stop bits;
	uint16_t word = ((uint16_t)c << 1) | (uint16_t) 0x600;

	//There are 8 + 1 + 2 = 11 bits in the message;

	//For all 11 bits :
	for (uint8_t bit = 11; bit--;) {

		//If bit is set :
		if (word & (uint16_t) 1) {

			//Logic high;
			debug_led_high();

		} else {

			//If bit is cleared, logic low;
			debug_led_low();

		}

		//Focus on next bit;
		word >>= 1;

		//Wait a bit time;
		debug_delay_us(bit_time);

	}

	//Stay in logic high;

}

void debug_echo(const char *msg) {

	//Determine the bit time for 9600 bauds;
	uint16_t bit_time = sw_uart_get_bit_time(9600);

	bit_time = 65535;
	//Cache the current char;
	char c;

	//While the current char is not null;
	while ((c = *(msg++))) {

		//Send it;
		sw_uart_send(bit_time, c);

	}


}

//Encode a char to an array of durations;
//char sw_uart_decode(uint16_t bit_time, const uint16_t *durations, size_t);
