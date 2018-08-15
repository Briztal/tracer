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

#include "modules/machine_control/OLDSHIT/tcontroller.h"


//--------------------------------------------------- Private headers --------------------------------------------------

//The function to stop the actuation layer;
void _actuation_stop(actuation_t *actuation);

//Query a new movement; returns true if succeeded;
bool actuation_query_movement(actuation_t *actuation);


//--------------------------------------------------- Inline --------------------------------------------------

/*
 * actuation_pause : disables the interrupt if required;
 */

inline void actuation_pause(actuation_t *actuation) {

	//If the actuation is started :
	if (actuation->state == ACTUATION_STARTED) {

		//Disable the interrupt;
		timer_interrupt_disable(&actuation->timer->reload_interrupt);

		//Update the state;
		actuation->state = ACTUATION_PAUSED;

	}

	//If not, nothing to do;

}


/*
 * actuation_resume : enables the interrupt; If in unregistered state, errors;
 */

inline void actuation_resume(actuation_t *actuation) {

	//If the actuation is unregistered:
	if (actuation->state == ACTUATION_STOPPED) {

		//Error, cannot resume from paused state;
		kernel_error("actuation.c : actuation_resume : the actuation layer is unregistered;");

	}

	//Enable the interrupt;
	timer_interrupt_enable(&actuation->timer->reload_interrupt);

	//Update the state;
	actuation->state = ACTUATION_STARTED;

}


//--------------------------------------------------- Initialisation --------------------------------------------------

/*
 * actuation_init : Initialises the timer pointer and enter in unregistered state;
 */

void actuation_init(actuation_t *actuation, timer_base_t *timer, tcontroller_t *tcontroller) {

	//Initialise the timer pointer;
	actuation->timer = timer;

	//Initialise the trajectory controller;
	actuation->tcontroller = tcontroller;

	//Stop the actuation;
	_actuation_stop(actuation);

}


//---------------------------------------------------- Start / stop ----------------------------------------------------

/*
 * actuation_start :
 * 	- updates the destination provider;
 * 	- starts the actuation layer;
 * 	- handler will be called immediately;
 */

void actuation_start(actuation_t *actuation) {

	//If the actuation is already started :
	if (actuation->state != ACTUATION_STOPPED) {

		//Error, can't be started twice;
		kernel_error("actuation.c : actuation_start : the actuation layer is not unregistered;");

	}

	//Query a new movement. Will stop is null provided;
	bool started = actuation_query_movement(actuation);

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
 * actuation_stop : stops the actuation layer, position accuracy is not guaranteed;
 */

void actuation_stop(actuation_t *actuation) {

	//TODO POSITION PATCH : FILL AN ARRAY WITH REALISED TICS;

	//Stop the actuation;
	_actuation_stop(actuation);

}


/*
 * _actuation_stop : resets the timer, resets dynamic data, enter in unregistered state;
 */

void _actuation_stop(actuation_t *actuation) {

	//If the layer is already unregistered, nothing to do;
	if (actuation->state == ACTUATION_STOPPED)
		return;

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

	//Mark the actuation unregistered;
	actuation->state = ACTUATION_STOPPED;

	//Reset the trajectory pointer;
	actuation->tcontroller = 0;

	//Reset the movement steps;
	actuation->mv_steps = 0;

}


//--------------------------------------------------- Movement update --------------------------------------------------

/*
 * actuation_query_movement : queries a new movement, stop if it is null, initialises the environment if not;
 */

bool actuation_query_movement(actuation_t *actuation) {

	//Cache the trajectory controller;
	tcontroller_t *const tcontroller = actuation->tcontroller;

	//Query a new movement;
	movement_t *next_movement = tcontroller_get_movement(tcontroller);

	//Cache the return state;
	bool correct_mv_received;

	//If the movement is null, the movement sequence is finished;
	if (!next_movement) {

		//We can stop the actuation layer;
		_actuation_stop(actuation);

		//Fail;
		correct_mv_received =  false;

	} else {
		//If the movement is not null :

		//Update the movement data;
		float period = (*(actuation->process_movement))(actuation, next_movement);

		//Update the timer's period;
		timer_set_period(actuation->timer, period);

		//Complete;
		correct_mv_received = true;

	}

	//If we must re-iterate :
	if (correct_mv_received) {

		//Notify the controller that it can discard the current movement;
		//At the same time, transmit the number of steps that will be made before next call;
		tcontroller_discard_movement(tcontroller, actuation->mv_steps);

	}

	//Return true if we received a correct movement;
	return correct_mv_received;

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
	 *	The procedure can be unregistered;
	 */
	if (!actuation->mv_steps) {

		//Stop the actuation layer;
		_actuation_stop(actuation);

	}

	//There is almost one more step to execute;
	actuation->sub_movement(actuation);

	//Decrement the remaining number of steps;
	actuation->mv_steps--;

	//Cache the trajectory pointer;
	tcontroller_t *trajectory = actuation->tcontroller;

	//If the movement is finished :
	if (!actuation->mv_steps) {

		//Query a new movement. Will stop is null provided;
		actuation_query_movement(actuation);

	} else {
		//If the movement is not finished :

		//Call for a sub_process;
		tcontroller_step_process(trajectory);

	}

}