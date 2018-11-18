/*
  pwm_n.c -  Part of TRACER

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

//----------------------------------------------- Module channel header ------------------------------------------------

#include <mod/module_channel.h>


//--------------------------------------------------- Make parameters --------------------------------------------------

#if !defined(CHANNEL_NAME) || !defined(PERIOD)  || !defined(TOLERANCE)

#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

#define CHANNEL_NAME pwm_0

#define PERIOD 20000

#define TOLERANCE 5

#endif

//------------------------------------------------------ Headers -------------------------------------------------------

#include <stdint.h>

#include "pwm_ch.h"


//-------------------------------------------------- Command if -------------------------------------------------

//Declare the duration update function;
void REFERENCE_SYMBOL(MODULE_NAME, update_channel_duration) (uint8_t channel_id, uint32_t high_duration);


/**
 * channel_update : calls the channel updater providing the channel id and the target duration;
 */

static void channel_update(uint32_t duration) {

	REFERENCE_SYMBOL(MODULE_NAME, update_channel_duration)(CHANNEL_ID, duration);

}


//Declare the duration update function;
void REFERENCE_SYMBOL(MODULE_NAME, disable_channel) (uint8_t channel_id);

/**
 * channel_update : calls the channel updater providing the channel id and the target duration;
 */

static void channel_disable() {

	REFERENCE_SYMBOL(MODULE_NAME, disable_channel)(CHANNEL_ID);

}


//Declare the duration update function;
void REFERENCE_SYMBOL(MODULE_NAME, isr_handler) (uint8_t channel_id);

/**
 * channel_update : calls the channel updater providing the channel id and the target duration;
 */

static void isr_handler() {

	REFERENCE_SYMBOL(MODULE_NAME, isr_handler)(CHANNEL_ID);

}


//--------------------------------------------- Channels interfaces ----------------------------------------------

/*
 * channels interfaces are stored in an array, and accessed by index during interfacing;
 */

//Initialise the if struct for a channel;
static struct channel_specs channel = {

	//Initialise the command if;
	.iface = {

		//The high level min and max will never change;
		.hl_min = 0,
		.hl_max = PERIOD,

		//Min and max depend on the timer configuration. They will be determined at init;

		//Update the set function;
		.set = &channel_update,

		.disable = &channel_disable,

	},

	.isr_handler = &isr_handler,

	//Save the channel file name;
	.channel_name = STR(CHANNEL_NAME),

	//Save the channel's maximal duration;
	.period_us = PERIOD,

	//Save the channel's tolerance;;
	.tolerance_us = TOLERANCE,

};


//Reference the channel;
MODULE_REFERENCE_CHANNEL(channel)

