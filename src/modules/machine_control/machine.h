/*
  machine.h Part of TRACER

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

#ifndef TRACER_MACHINE_H
#define TRACER_MACHINE_H

#include <stdbool.h>

#include <stdint.h>

#include "geometry.h"

#include "movement.h"


//------------------------------------------- Machine constants -------------------------------------------

/*
 * The machine constants structure. Contains constant machine parameters;
 */

typedef struct {

	//The machine's dimension;
	const uint8_t dimension;

	//The machine's geometry;
	const geometry_t *const geometry;

} machine_constants_t;


//------------------------------------------- Machine state -------------------------------------------

/*
 * A machine state contains all data that describe the real time state of the machine;
 *
 * 	It comprises a mandatory part, that presents the actuation layer's current state, and a custom part,
 * 	that is left to the controller's implementation;
 */

typedef struct {

	//Actuation positions. Actuators commanded on an integer base;
	int32_t *const actuation_positions;

	//Actuation speeds. Time is expressed in float, so are speeds;
	float *const actuation_speeds;

	//Control positions. Expressed in float;
	float *const control_positions;

	//The controller's state; Can contain whatever the controller decides;
	void *const controller_state;

} machine_state_t;


/*
 * Sometimes, some members can't be edited. Following structs provides restricted access to some members;
 */
typedef struct {

	int32_t *const actuation_positions;
	const float *const actuation_speeds;
	float *const control_positions;
	const void *const controller_state;

} machine_state_p_t;


typedef struct {

	const int32_t *const actuation_positions;
	const float *const actuation_speeds;
	const float *const control_positions;
	void *const controller_state;

} machine_state_l_t;


typedef struct {

	const int32_t *const actuation_positions;
	const float *const actuation_speeds;
	const float *const control_positions;
	const void *const controller_state;

} machine_state_const_t;



//------------------------------------------- Time interval -------------------------------------------

/*
 * To describe durations, we here define time interval type;
 */

typedef struct {

	//The minimum. Exists because the interval is closed; The minimum always exists, as duration is positive;
	float min;

	//The maximum. Exists because the interval is closed;
	float max;

	//The maximum doesn't always exist. This flag is set when it actually does;
	bool max_exists;

	//A useful flag that contains the value of "min > max" if related, or simply tells that the interval is empty;
	bool empty;

	//A useful flag, to mention that the interval is invalid in the considered circumstances;
	bool valid;

} time_interval_t;



//------------------------------------------- Movement builder -------------------------------------------

/*
 * Movements are computed step by step, and temporary data is stored in the movement builder.
 */

typedef struct {

	//The movement that we currently compute; References the distances array and the duration;
	movement_t *movement;

	//The time interval array, containing the duration window for each actuator during the computation;
	time_interval_t *interval_array;

	//The duration interval that satisfies all constraints;
	time_interval_t duration_interval;

	//After distances were provided, they might be modified to avoid breaking actuation limitations. This flag is
	//set when they are modified;
	bool distances_altered;

	//The controller builder. Can contain whatever the controller decides;
	void *controller_builder;

} movement_buinlder_t;


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

#endif //TRACER_MACHINE_H
