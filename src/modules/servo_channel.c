/*
  servo.c -  Part of TRACER

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

#include <stdint.h>

#include <kernel/mod/module_channel.h>
#include "servo_channel.h"


//--------------------------------------------------- Make parameters --------------------------------------------------


#if !defined(CHANNEL_NAME) || !defined(GPIO_NAME) || !defined(MAX_DURATION)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

#define CHANNEL_NAME servo_0

#define GPIO_NAME gpio

#define MAX_DURATION 20000

#endif


//-------------------------------------------------- Command interface -------------------------------------------------

//Declare the duration update function;
void update_channel_duration(uint32_t *, uint32_t);


/**
 * channel_update : calls the channel updater providing the channel id and the target duration;
 */

static void channel_update(uint32_t duration) {

	update_channel_duration(CHANNEL_ID, duration);

}


//--------------------------------------------- Channels interfaces ----------------------------------------------

/*
 * channels interfaces are stored in an array, and accessed by index during interfacing;
 */

//Initialise the interface struct for a channel;
static struct channel_specs channel = {

	//Initialise the command interface;
	.iface = {

		//Update the set function;
		.set = &channel_update,

	},

	//Save the gpio file name;
	.gpio_name = STR(GPIO_NAME),

	//Save the channel file name;
	.gpio_name = STR(CHANNEL_NAME),

	//Save the channel's maximal duration;
	.max_duration = MAX_DURATION,

};

//Reference the channel;
MODULE_REFERENCE_CHANNEL(channel);






