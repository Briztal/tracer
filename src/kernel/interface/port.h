/*
  port.h Part of TRACER

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

#ifndef TRACER_PORT_H
#define TRACER_PORT_H

#include <stdint.h>

//--------------------------------------------------- Port pin config --------------------------------------------------

/*
 * Below are listed common configuration options that port chip are free to use in their configuration structures;
 */

/*
 * Types of interruptions;
 */

enum port_interrupt_t {

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

enum port_pin_direction_t {

	//Pin is used to collect data;
		PORT_INPUT,

	//Pin is used to provide data;
		PORT_OUTPUT

};


/*
 * Different input hardware configurations;
 */

enum port_input_mode_t {

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

enum port_output_mode_t {

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

enum port_slew_rate_t {

	//Low slew rate;
		PORT_LOW_RATE,

	//High slew rate;
		PORT_HIGH_RATE,

};


/*
 * The basic configuration structure for a pin;
 */

struct port_pin_config {

	/*
	 * Hardware configuration;
	 */

	//The enabled multiplexing channel;
	uint8_t mux_channel;

	//The data direction;
	enum port_pin_direction_t direction;


	/*
	 * Input hardware configuration
	 */

	//The input mode;
	enum port_input_mode_t input_mode;


	/*
	 * Output hardware configuration
	 */

	//The output mode;
	enum port_output_mode_t output_mode;

	//The slew rate;
	enum port_slew_rate_t slew_rate;


	/*
	 * Interruption;
	 */

	//The type of interruption;
	enum port_interrupt_t interrupt_type;

	//The function to call in case of interrupt selected;
	void (*interrupt_function)(void);

};




#endif //TRACER_PORT_H
