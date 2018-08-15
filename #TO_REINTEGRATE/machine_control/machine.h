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

#include "actuation_physics.h"


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




#endif //TRACER_MACHINE_H
