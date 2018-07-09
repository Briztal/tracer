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
	int32_t *actuation_positions;

	//Actuation speeds. Time is expressed in float, so are speeds;
	float *actuation_speeds;

	//The controller's state; Can contain whatever the controller decides;
	void *controller_state;

} machine_state_t;


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

	//A useful flag that contains the value of "min == max";
	bool singleton;

	//A useful flag that contains the value of "min > max";
	bool empty;

} time_interval_t;



#endif //TRACER_MACHINE_H
