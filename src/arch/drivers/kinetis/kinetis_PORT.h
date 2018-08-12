/*
  kinetis_PORT.h Part of TRACER

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

#ifndef TRACER_KINETIS_PORT_H
#define TRACER_KINETIS_PORT_H


#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include <kernel/arch/drivers/port.h>

#include <kernel/memory/memory_transfer.h>


//----------------------------------------------- Special configurations -----------------------------------------------

/*
 * Different types of hardware filtering;
 */

enum kinetis_PORT_input_filter_t{

    //No filtering enabled;
            PORT_NO_FILTERING,

    //Digital filtering enabled;
            PORT_DIGITAL_FILTERING,

    //Passive filtering enabled;
            PORT_PASSIVE_FILTERING,

};


//----------------------------------------------- Configuration structure ----------------------------------------------

/*
 * The basic configuration structure for a pin;
 */

struct kinetis_pin_configuration_t {

	/*
	 * Interruption;
	 */

	//The type of interruption;
	enum PORT_interrupt_t interrupt_type;

	//The function to call in case of interrupt selected;
	void (*interrupt_function)(void);


	/*
	 * Hardware configuration;
	 */

	//The enabled multiplexing channel;
	uint8_t mux_channel;

	//The data direction;
	enum PORT_pin_direction_t direction;


	/*
	 * Input hardware configuration
	 */

	//The input mode;
	enum PORT_input_mode_t input_mode;


	/*
	 * Output hardware configuration
	 */

	//The output mode;
	enum PORT_output_mode_t output_mode;

	//The slew rate;
	enum PORT_slew_rate_t slew_rate;


	/*
	 * Filtering hardware configuration
	 */

	//The type of input filtering;
	enum kinetis_PORT_input_filter_t input_filter;

	//The length of the eventual digital filtering;
	uint8_t filtering_length;

};


//--------------------------------------------- Kinetis PORT driver struct ---------------------------------------------

/*
 * The kinetis PORT driver is composed of the following set of elements :
 */

struct kinetis_PORT_driver_t {

	//The general PORT driver, for pointer cast;
	struct port_driver port_driver;


	/*
	 * The kinetis PORT and GPIO are well designed. They are separate drivers (their only interaction residing
	 * 	in the signal direction, that the GPIO controls;
	 *
	 * 	Peripherals of the same type are mapped in constantly spaced consecutive regions of the RAM;
	 *
	 * 	Their locations are stored in memory descriptions, each peripheral zone being considered as a bloc;
	 */

	//The structure that references all consecutive port drivers;
	const struct mem_desc_t gpios;

	//The structure that references all consecutive gpio drivers;
	const struct mem_desc_t ports;

};


//------------------------------------------------- Creation - Deletion ------------------------------------------------

//Create a kinetis PORT driver;
struct kinetis_PORT_driver_t *kinetis_PORT_create(volatile void *first_port_peripheral,
												  volatile void *first_gpio_peripheral,
												  uint8_t nb_peripherals,
												  size_t port_memory_spacing, size_t gpio_memory_spacing);



#endif //TRACER_KINETIS_PORT_H
