/*
  actuation.h Part of TRACER

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


#ifndef TRACER_ACTUATION_H
#define TRACER_ACTUATION_H

#include "stdint.h"

#include "trajectory.h"

#include <kernel/drivers/timer.h>

/*
 * The actuation layer interfaces with actuators; Its goals are :
 * 	- To go to the required position in the required duration;
 * 	- To divide the required movement into sub-movements if necessary;
 */

/*
 * An actuation layer can be started or stopped, depending on if it has a movement to execute;
 */
typedef enum {

	ACTUATION_STARTED,

	ACTUATION_STOPPED,
	//TODO ?

} actuation_state_t;


//To describe boolean qualifications over an array of elements, we define the signature boolean type;
typedef uint32_t sig_t;


/*
 * A destination position is represented by a set of coordinates and a delay to reach;
 */
typedef struct {

	//The number of axis of the movement;
	uint8_t size;

	//The distances coordinates array;
	uint16_t *distances;

	//The directions signature; signature is negative.
	sig_t sign_signature;

	//The speed coordinates array;
	//TODO ?

	//The number of microseconds till position must be reached;
	float time_to_dest;

} elementary_movement_t;


typedef struct actuation_t {

	//The timer that will be used to time the movement;
	timer_base_t *timer;

	//The trajectory object we rely on;
	trajectory_controller_t *trajectory;

	//The current state;
	actuation_state_t state;

	//The maximal distance target for one movement;
	uint16_t distance_target;


	/*
	 * Data for the actuation function;
	 */

	//The number of steps of the current movement;
	uint16_t mv_steps;


	/*
	 * Implementation reserved
	 */

	//The function to call after the reception of a destination position;
	float (*const process_movement)(struct actuation_t *, elementary_movement_t *);

	//The actuation function, reserved to implementation;
	void (*const sub_movement)(struct actuation_t *);


} actuation_t;


//Initialise the actuation layer, enter in stopped state;
void actuation_init(actuation_t *actuation, timer_base_t *timer, uint16_t distance_target);

//Start the actuation layer, handler will be called immediately;
void actuation_start(actuation_t *actuation, trajectory_controller_t *);

//Abort the current movement; An offset may happen depending on the implementation;
void actuation_abort(actuation_t *actuation);

//The actuation handler;
void actuation_handler(actuation_t *actuation);


#endif //TRACER_ACTUATION_H
