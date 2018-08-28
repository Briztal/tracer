/*
  PORT.h Part of TRACER

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

#ifndef TRACER_PORT_H
#define TRACER_PORT_H

#include "stdint.h"


//---------------------------------------------------- IO descriptor ---------------------------------------------------

/*
 * To configure IOs correctly, we need the following set of information, provided by the board header;
 */

struct io_desc_t {

	//The port identifier;
	uint8_t port_index;

	//The bit identifier;
	uint8_t bit_index;

	//The mux channel;
	uint8_t mux_channel;

};


//---------------------------------------------------- GPIO data ---------------------------------------------------

/*
 * GPIOs are manipulated directly by register. When a GPIO operation is required, the register's address is queried,
 * depending of the type of operation required;
 *
 * A GPIO operation register can have the following type :
 */

enum gpio_register_type_e {

	//Complete port write;
		GPIO_OUTPUT_DATA_REGISTER,

	//Set a certain number of bits;
		GPIO_OUTPUT_SET_REGISTER,

	//Clear a certain number of bits;
		GPIO_OUTPUT_CLEAR_REGISTER,

	//Toggle a certain number of bits;
		GPIO_OUTPUT_TOGGLE_REGISTER,

	//Complete port read;
		GPIO_INPUT_DATA_REGISTER,

};

//---------------------------------------------------- Port Driver ---------------------------------------------------


/*
 * The PORT driver has two objectives :
 * 	- configuring pins to their required mode;
 * 	- providing access to GPIO registers;
 */

struct port_driver {

	//------------------- Pin config -------------------

	//Get a pin's current configuration (useful to defaults mistakes);
	void (*const get_pin_config)(const struct port_driver *, const struct io_desc_t *pin, void *config);

	//Set a pin's configuration. Mux channel is updated automatically. Config might have been queried before;
	void (*const configure_pin)(const struct port_driver *, const struct io_desc_t *pin, void *config);



	//------------------- Gpio -------------------

	//The size of a gpio register;
	const size_t gpio_register_size;

	//Get the descriptor for a single IO pin : the register and the appropriate mask for the pin
	bool (*const get_gpio_descriptor)(const struct port_driver *,
								 enum gpio_register_type_e type, const struct io_desc_t *,
								 volatile void **gpio_register, void *mask_p);


	//------------------- Destructor -------------------

	//The function that must be called to delete the port driver;
	void (*const destructor)(struct port_driver *);

};


//Shortcut for pin config getter;
static inline void port_driver_get_pin_config(const struct port_driver *driver, const struct io_desc_t *pin, void *config) {
	(*(driver->get_pin_config))(driver, pin, config);
}


//Shortcut for pin config setter;
static inline void port_driver_configure_pin(const struct port_driver *driver, const struct io_desc_t *pin, void *config) {
	(*(driver->configure_pin))(driver, pin, config);
}


//Shortcut for gpio descriptor getter;
static inline void port_driver_get_gpio_descriptor(const struct port_driver *driver,
									 enum gpio_register_type_e type, const struct io_desc_t *pin,
									 volatile void **gpio_register, void *mask_p) {
	(*(driver->get_gpio_descriptor))(driver, type, pin, gpio_register, mask_p);
}

//Shortcut for destruction
inline void port_driver_delete(struct port_driver *driver) {
	(*(driver->destructor))(driver);
}


//--------------------------------------------------- Port pin config --------------------------------------------------

/*
 * Below are listed common configuration options that port chip are free to use in their configuration structures;
 */


/*
 * Types of interruptions;
 */

enum PORT_interrupt_t {

	//No interruption enabled;
		PORT_NO_INTERRUPT,

	//DMA request on rising edge;
		PORT_DMA_RISING_EDGE,

	//DMA request on falling edge;
		PORT_DMA_FALLING_EDGE,

	//DMA request on rising or falling edge;
		PORT_DMA_EDGE,

	//Interrupt request when logical 0;
		PORT_INTERRUPT_0,

	//Interrupt request when logical 1;
		PORT_INTERRUPT_1,

	//Interrupt request on rising edge;
		PORT_INTERRUPT_RISING_EDGE,

	//Interrupt request on falling edge;
		PORT_INTERRUPT_FALLING_EDGE,

	//Interrupt request on rising or falling edge;
		PORT_INTERRUPT_EDGE,

};


/*
 * The hardware data direction;
 */

enum PORT_pin_direction_t {

	//Pin is used to collect data;
		PORT_INPUT,

	//Pin is used to provide data;
		PORT_OUTPUT

};


/*
 * Different input hardware configurations;
 */

enum PORT_input_mode_t {

	//High impedance configuration;
		PORT_HIGH_IMPEDANCE,

	//Pull-up configuration;
		PORT_PULL_UP,

	//Pull-down configuration;
		PORT_PULL_DOWN,

	//Hysteresis configuration;
		PORT_HYSTERSIS,


	//Repeater configuration;
		PORT_REPEATER,

};


/*
 * Different hardware output configurations;
 */

enum PORT_output_mode_t {

	//Open drain configuration;
		PORT_OPEN_DRAIN,

	//Push-pull configuration;
		PORT_PUSH_PULL,

	//High current drive configuration;
		PORT_HIGH_DRIVE,

};


/*
 * Different slew rates;
 */

enum PORT_slew_rate_t {

	//Low slew rate;
		PORT_LOW_RATE,

	//High slew rate;
		PORT_HIGH_RATE,

} PORT_slew_rate_t;



#endif //TRACER_PORT_H
