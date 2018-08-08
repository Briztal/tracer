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
#include <kernel/net/framer/framer.h>
#include <kernel/net/framer/ascii_framer.h>

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

	//Create the framer;
	struct data_framer *framer = ascii_framer_create();

	//Create the config file, with the framer ownership transferred; TODO PASS THE FRAMER CREATOR, SAFER;
	struct UART_config_t config = UART_DEFAULT_CONFIG(framer);

	kinetis_UART_start(UART0, &config);

	while(true) {

		if (netf2_message_available(UART0->iface)) {

			uint8_t t[100];

			struct data_block block = {
				.head = {},
				.address = &t,
				.max_size = 100,
				.size = 0,
			};

			list_init((struct list_head *) &block.head);

			netf2_get_frame(UART0->iface, &block);

			netf2_send_frame(UART0->iface, &block);

		}

	}


    core_init();

    //Never reached;
    while (true);


}



