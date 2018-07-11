/*
  movement_builder.c Part of TRACER

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

#include "movement_builder.h"

#include <string.h>

#include <kernel/kernel.h>


/*
 * movement_builder_initialise : initialises a movement builder for a machine of a given dimension;
 */

void movement_builder_initialise(movement_builder_t *const builder, const uint8_t dimension,
								 void *const controller_builder) {

	//Allocate memory for the duration array;
	time_interval_t *const interval_array = kernel_malloc(dimension * sizeof(time_interval_t));

	//Create an initializer for arrays;
	time_interval_t interval_init = {
		.min = 0,
		.max_exists = false,
		.max = 0,
		.valid = true,
		.empty = false,
	};

	//For each interval :
	for (uint8_t actuator_index = 0; actuator_index < dimension; actuator_index++) {

		//Initialise the interval to the largest possible one;
		interval_array[actuator_index] = interval_init;

	}

	//Transfer the ownership of the interval array;
	movement_builder_t init = {

		//No movement is attributed for instance;
		.movement = 0,

		//Transfer the ownership of the interval array;
		.interval_array = interval_array,

		//Initialise the duration interval to the largest width;
		.duration_interval = interval_init,

		//Transfer the ownership of the controller builder part;
		.controller_builder = controller_builder,

	};

	//Initialise the builder;
	memcpy(builder, &init, sizeof(movement_builder_t));

}


/*
 * movement_builder_delete : deletes the interval array of the movement builder;
 *
 * 	The references movement is not owned, and so is not deleted, as for the controller builder, that must be
 * 	deleted by the controller;
 */

void movement_builder_delete(movement_builder_t *builder) {

	//Delete only the interval array;
	kernel_free(builder->interval_array);

}