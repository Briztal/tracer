/*
  uart.h Part of TRACER

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

#ifndef TRACER_UARTDRIVER_H
#define TRACER_UARTDRIVER_H

#include <net/netf.h>

#include <stdint.h>

#include "stdbool.h"

#include "driver.h"


/*
 * Number of possible data bits;
 */

typedef enum {
    EVEN_PARITY,
    ODD_PARITY
} parity_bit_t;


/*
 * The type of the transmission;
 */

typedef enum {
    HALF_DUPLEX,
    FULL_DUPLEX
} transmission_type_t;


/*
 * The buffer watermark policy;
 */

typedef enum {
    STRICT,
    HALF_REMAINING
} watermark_policy_t;




/*
 * The UART configuration structure;
 */

struct UART_config_t {


    /*--------------------- L1 config ---------------------*/

    /*The Number of data bits;*/
    uint8_t nb_data_bits;

    /*Is the parity bit present ?*/
    bool parity_bit_enabled;

    /*The type of the parity bit;*/
    parity_bit_t parity_type;

    /*RTS enabled;*/
    bool rts_enabled;

    /*CTS check before detection enabled;*/
    bool cts_enabled;

    /*The transmission sequences_initialised;*/
    transmission_type_t transmission_type;

    /*The Baudrate;*/
    uint32_t baudrate;


	/*--------------------- L2 config ---------------------*/

	/*The framer. Ownership transferred;*/
	struct data_framer *framer;

	/*The maximal size of all frames;*/
	size_t max_frame_size;

	/*The maximal number of frames stored by the if;*/
	size_t nb_frames;

};


#define UART_DEFAULT_CONFIG(_framer)  \
		{\
            .nb_data_bits = 8,\
            .parity_bit_enabled = false,\
            .parity_type = EVEN_PARITY,\
            .rts_enabled = false,\
            .cts_enabled = false,\
            .transmission_type = FULL_DUPLEX,\
            .baudrate = 9600,\
			.framer = (_framer),\
			.max_frame_size = 100, \
			.nb_frames = 5,\
    	}


#endif /*TRACER_UARTDRIVER_H*/
