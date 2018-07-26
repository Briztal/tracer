/*
  Part of TRACER

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

/*
#include "kernel/kernel.h"
#include "setjmp.h"

 */


#include <kernel/arch/arch.h>

#include <kernel/arch/peripherals/kinetis/kinetis_UART.h>
#include <kernel/kernel.h>
#include <HardwareSerial.h>
#include <kernel/arch/peripherals/kinetis/kinetis_mux.h>
#include <core_pins.h>

#define BAUD2DIV(baud)  (((F_CPU * 2) + ((baud) >> 1)) / (baud))

int main() {

	/*
	serial_begin(BAUD2DIV(9600));
	while(true) {

		serial_print("SUUS");

		teensy35_delay(1000);
	}

*/

	//Initialise drivers;
	teensy35_hardware_init();


	struct port_driver_t *port = (struct port_driver_t *)PORT;

	//Cache a pin configuration for pin C5 (LED);
	struct kinetis_pin_configuration_t pin_configuration;

	//Cache the UART TX pin;
	struct io_desc_t pin = UART0_TX;

	//Get the current configuration;
	port_driver_get_pin_config(port, &pin, &pin_configuration);

	//Update the configuration for UART
	pin_configuration.direction = PORT_OUTPUT;
	pin_configuration.output_mode = PORT_HIGH_DRIVE;
	pin_configuration.slew_rate = PORT_HIGH_RATE;

	//Update the configuration;
	port_driver_configure_pin(port, &pin, &pin_configuration);

	//Cache the UART RX pin;
	pin = (struct io_desc_t) UART0_RX;

	//Get the current configuration;
	port_driver_get_pin_config(port, &pin, &pin_configuration);

	//Update the configuration for UART
	pin_configuration.direction = PORT_INPUT;
	pin_configuration.input_mode = PORT_PULL_DOWN;
	pin_configuration.input_filter = PORT_PASSIVE_FILTERING;

	//Update the configuration;
	port_driver_configure_pin(port, &pin, &pin_configuration);


	struct UART_config_t config = UART_DEFAULT_CONFIG();

	kinetis_UART_start(UART0, &config);

	struct kinetis_UART_stream_t *i_stream = UART0->input_stream;
	struct kinetis_UART_stream_t *o_stream = UART0->output_stream;

	char st[32] = {48, 48, 48, 48, 48, 48, 48, 48, 48};
	struct mem_desc_t mem = {
		.memory_map = {
			.start_address = &st,
			.block_spacing = 1,
		},

		.bloc_desc = {
			.block_size = 1,
			.nb_blocks = 32,
		}
	};


	struct blocks_desc_t desc;

	while(true) {

		size_t count = stream_read((struct stream_memory_t *) o_stream, &mem);

		if (count) {

			if (count > 32) count = 32;
			mem.bloc_desc.nb_blocks = count << 1;
			count = stream_write((struct stream_memory_t *) i_stream, &mem);

		}

		teensy35_led_high();
		teensy35_delay(250);
		teensy35_led_low();
		teensy35_delay(250);

		/*
		(*(stream->stream.get_bloc_descriptor))((struct stream_memory_t *) stream, &desc);

		if (desc.nb_blocks != 8) {
			teensy35_led_blink(50);
		}
		 */


	}
	teensy35_led_count(desc.nb_blocks + 1);



    core_init();

    //Never reached;
    while (true);


}



