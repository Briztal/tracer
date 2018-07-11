/*
  movement_builder.h Part of TRACER

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

#ifndef TRACER_MOVEMENT_BUILDER_H
#define TRACER_MOVEMENT_BUILDER_H

#include "machine.h"

#include <stdint.h>

//------------------------------------------- Movement builder -------------------------------------------

/*
 * Movements are computed step by step, and temporary data that is not related to a machine state
 * 	is stored in the movement builder.
 */

typedef struct {

	//The movement that we currently compute; References the distances array and the duration;
	movement_t *movement;

	//The time interval array, containing the duration window for each actuator during the computation;
	time_interval_t *const interval_array;

	//The duration interval that satisfies all constraints;
	time_interval_t duration_interval;

	//The controller builder. Can contain whatever the controller decides;
	void *const controller_builder;

} movement_builder_t;


/*
 * Restricted access builder structs :
 */


//controller restricted struct
typedef struct {
	const movement_t *const movement;
	const time_interval_t *const interval_array;
	const time_interval_t duration_interval;
	const bool distances_altered;
	void *const controller_builder;
} movement_builder_r_t;


//Immutable struct
typedef struct {
	const movement_t *const movement;
	const time_interval_t *const interval_array;
	const time_interval_t duration_interval;
	const bool distances_altered;
	const void *const controller_builder;
} movement_builder_const_t;


//---------------------------------------------------- Init - Delete ---------------------------------------------------

//Initialise a movement builder for a machine of a given dimension;
void movement_builder_initialise(movement_builder_t *builder, uint8_t dimension, void *control_builder);

//Delete a created movement builder;
void movement_builder_delete(movement_builder_t *builder);


#endif //TRACER_MOVEMENT_BUILDER_H
