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


#include "kernel/syscall.h"
#include "setjmp.h"



#include <kernel/arch/arch.h>

#include <kernel/arch/drivers/kinetis/kinetis_UART.h>
#include <kernel/arch/drivers/kinetis/kinetis_mux.h>
#include <kernel/net/framer/ascii_framer.h>
#include <core/debug.h>
#include <data_structures/string/string.h>
#include <core/driver/timer.h>
#include <kernel/arch/drivers/port.h>
#include <kernel/arch/drivers/kinetis/kinetis_PORT.h>
#include <kernel/arch/drivers/kinetis/kinetis_PIT.h>


void kernel_init_function(void *args);

int main() {

	//Initialise chip;
	teensy35_hardware_init();

	core_init();

	//kernel_init_function(0);

	//Never reached;
	while (true);

}

bool PIT_log_flag = true;


void PIT_log() {

	timer_clr_int_flag(PIT->ifaces[0]);

	bool flag = PIT_log_flag;


	if (flag) {
		debug_led_high();
	} else {
		debug_led_low();
	}

	PIT_log_flag = !flag;

}


struct netf2 *uart_init() {

	struct port_driver *port = (struct port_driver *) PORT;

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

	return (struct netf2 *) UART0->iface;

}



void PIT_test() {


	uint32_t t;


	driver_init((struct kernel_driver *) PIT, &t);

	struct timer_interface *interface = PIT->ifaces[0];

	struct kinetis_PIT_timer_interface *ifa = (struct kinetis_PIT_timer_interface *) interface;

	//debug_led_cmp((size_t)ifa->registers != 0x40037100);

	//Set the overflow value to 100ms;
	timer_set_ovf_value(interface, 1000);

	//Update the handler;
	timer_update_handler(interface, &PIT_log);

	//Start the timer;
	timer_start(interface);

	//Enable the interrupt
	timer_enable_int(interface);

	//timer_stop(interface);


	struct netf2 *iface = uart_init();


	while (true) {

		//string_t *str = str_u32(timer_started(interface));
		string_t *str = str_u32(timer_get_count(interface));
		//string_t *str = str_u32(timer_get_ovf_value(interface));

		struct data_block block = {
			.head = {},
			.address = str->data,
			.max_size = str->length,
			.size = str->length,
		};

		list_init((struct list_head *) &block.head);

		netf2_send_frame(iface, &block);

		string_delete(str);

		debug_delay_ms(100);

	}

	//debug_led_cmp(timer_int_enabled(interface));

	//TODO TESTER TOUTES LES FONCTIONS DE LA LIB TIMER;

	//debug_led_cmp(timer_started(interface));

}


void uart_send_u() {

	struct netf2 *iface = uart_init();

	while (true) {

		string_t *str = str_u32((uint32_t) 4135468579);

		struct data_block block = {
			.head = {},
			.address = str->data,
			.max_size = str->length,
			.size = str->length,
		};

		list_init((struct list_head *) &block.head);

		netf2_send_frame(iface, &block);

		string_delete(str);

		debug_delay_ms(500);

	}

}


void uart_loopback() {

	struct netf2 *iface = uart_init();

	while (true) {

		if (netf2_message_available(iface)) {

			uint8_t t[100];

			struct data_block block = {
				.head = {},
				.address = &t,
				.max_size = 100,
				.size = 0,
			};

			list_init((struct list_head *) &block.head);

			netf2_get_frame(iface, &block);

			netf2_send_frame(iface, &block);

		}

	}
}

void kernel_init_function(void *args) {

	//uart_loopback();

	PIT_test();

	while (true);

}