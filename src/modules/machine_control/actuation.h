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

#include "modules/machine_control/OLDSHIT/tcontroller.h"

#include <kernel/drivers/timer.h>

/*
 * The actuation layer interfaces with actuators; Its goals are :
 * 	- To go to the required position in the required duration;
 * 	- To divide the required movement into sub-trajectories if necessary;
 */

/*
 * An actuation layer can be started or unregistered, depending on if it has a movement to execute;
 */
typedef enum {

	//Timer is started, interrupt is enabled;
	ACTUATION_STARTED,

	//Timer is started, interrupt is disabled;
	ACTUATION_PAUSED,

	//Timer is unregistered, interrupt is disabled;
	ACTUATION_STOPPED,

} actuation_state_t;


typedef struct actuation_t {

	//The timer that will be used to time the movement;
	timer_base_t *timer;

	//The trajectory object we rely on;
	tcontroller_t *tcontroller;

	//The current state;
	actuation_state_t state;


	/*
	 * Data for the actuation function;
	 */

	//The number of steps of the current movement;
	uint16_t mv_steps;


	/*
	 * Implementation reserved
	 */

	//The function to call after the reception of a destination position;
	float (*const process_movement)(struct actuation_t *, movement_t *);

	//The actuation function, reserved to implementation;
	void (*const sub_movement)(struct actuation_t *);


} actuation_t;


//Initialise the actuation layer, enter in unregistered state;
void actuation_init(actuation_t *actuation, timer_base_t *timer, tcontroller_t *tcontroller);

//Start the actuation layer, handler will be called immediately;
void actuation_start(actuation_t *actuation);

//Abort the current movement; An offset may happen depending on the implementation;
void actuation_stop(actuation_t *actuation);

//The actuation handler;
void actuation_handler(actuation_t *actuation);


#endif //TRACER_ACTUATION_H
