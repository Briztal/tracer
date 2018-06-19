/*
  actuation.c Part of TRACER

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

#include <kernel/kernel.h>

#include "actuation.h"

#include "trajectory.h"


//--------------------------------------------------- Private headers --------------------------------------------------

//The function to stop the actuation layer;
void actuation_stop(actuation_t *actuation);

//Query a new movement; returns true if succeeded;
bool actuation_query_movement(actuation_t *actuation, trajectory_controller_t *trajectory);


//--------------------------------------------------- Initialisation --------------------------------------------------

/*
 * actuation_init : Initialises the timer pointer and enter in stopped state;
 */

void actuation_init(actuation_t *actuation, timer_base_t *timer, uint16_t distance_target) {

	//Initialise the timer pointer;
	actuation->timer = timer;

	//Initialise the distance target;
	actuation->distance_target = distance_target;

	//Stop the actuation;
	actuation_stop(actuation);

}


//---------------------------------------------------- Start / stop ----------------------------------------------------

/*
 * actuation_start :
 * 	- updates the destination provider;
 * 	- starts the actuation layer;
 * 	- handler will be called immediately;
 */

void actuation_start(actuation_t *actuation, trajectory_controller_t *trajectory) {

	//If the actuation is already started :
	if (actuation->state == ACTUATION_STARTED) {

		//Error, can't be started twice;
		kernel_error("actuation.c : actuation_start : the actuation layer is already started;");

	}

	//Initialise the trajectory instance;
	actuation->trajectory = trajectory;

	//Query a new movement. Will stop is null provided;
	bool started = actuation_query_movement(actuation, trajectory);

	//If query succeeded :
	if (started) {

		//Mark the actuation started;;
		actuation->state = ACTUATION_STARTED;

		//The timer's period/value has already been updated;

		//Cache the timer;
		const timer_base_t *const timer = actuation->timer;

		//Start the timer;
		timer_start_timer(timer);

		//Enable the interrupt;
		timer_interrupt_enable(&timer->reload_interrupt);

	}

}


/*
 * actuation_abort : stops the actuation layer, position accuracy is not guaranteed;
 */

void actuation_abort(actuation_t *actuation) {

	//TODO POSITION PATCH : FILL AN ARRAY WITH REALISED TICS;

	//Stop the actuation;
	actuation_stop(actuation);

}


/*
 * actuation_stop : resets the timer, resets dynamic data, enter in stopped state;
 */

void actuation_stop(actuation_t *actuation) {

	//Cache the timer;
	const timer_base_t *const timer = actuation->timer;

	//Cache the reload interrupt;
	const timer_interrupt_data_t *const interrupt = &timer->reload_interrupt;

	//Disable the interrupt;
	timer_interrupt_disable(interrupt);

	//Clear the flag;
	timer_interrupt_flag_clear(interrupt);

	//Stop the timer;
	timer_stop_timer(timer);

	//Mark the actuation stopped;
	actuation->state = ACTUATION_STOPPED;

	//Reset the trajectory pointer;
	actuation->trajectory = 0;

	//Reset the movement steps;
	actuation->mv_steps = 0;

}


//--------------------------------------------------- Movement update --------------------------------------------------

/*
 * actuation_query_movement : queries a new movement, stop if it is null, initialises the environment if not;
 */

bool actuation_query_movement(actuation_t *actuation, trajectory_controller_t *trajectory) {

	//Query a new movement;
	elementary_movement_t *next_movement = trajectory_get_elementary_movement(trajectory);

	//If the movement is null, the movement sequence is finished;
	if (!next_movement) {

		//We can stop the actuation layer;
		actuation_stop(actuation);

		//Fail;
		return false;

	} else {
		//If the movement is not null :

		//Update the movement data;
		float period = (*(actuation->process_movement))(actuation, next_movement);

		//Update the timer's period;
		timer_set_period(actuation->timer, period);

		//Transmit the number of steps the movement will comprise to the trajectory manager;
		trajectory_update_movement_steps(trajectory, actuation->mv_steps);

		//Complete;
		return true;

	}

}


//------------------------------------------------ Interruption handler ------------------------------------------------

/*
 * actuation_handler : the function called to move, and prepare the next movement;
 */

void actuation_handler(actuation_t *const actuation) {

	/*
	 * If there are no more steps to execute :
	 *	- the movement is finished;
	 *	- no more movement are available (check at previous iteration);
	 *
	 *	The procedure can be stopped;
	 */
	if (!actuation->mv_steps) {

		//Stop the actuation layer;
		actuation_stop(actuation);

	}

	//There is almost one more step to execute;
	actuation->sub_movement(actuation);

	//Decrement the remaining number of steps;
	actuation->mv_steps--;

	//Cache the trajectory pointer;
	trajectory_controller_t *trajectory = actuation->trajectory;

	//If the movement is finished :
	if (!actuation->mv_steps) {

		//Query a new movement. Will stop is null provided;
		actuation_query_movement(actuation, trajectory);

	} else {
		//If the movement is not finished :

		//Call for a sub_process;
		trajectory_sub_process(trajectory);

	}

}