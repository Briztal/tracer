/*
  mcontroller.c Part of TRACER

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

#include "mcontroller.h"

#include "kernel/computation/staged_execution.h"
#include "movement.h"
#include "machine.h"


void _mcontroller_compute_duration_interval(actuation_speed_constraint_t *constraint, time_interval_t *interval);

void mcontroller_merge_intervals(const time_interval_t *intervals, time_interval_t *const merges_interval);

void mcontroller_correct_interval(time_interval_t *const final_interval);

/*
 * mcontroller_initialise_calculation : this function is the first step of the movement computation.
 *
 * 	It initialises the computation procedure, namely  :
 * 	- Resets the position computer;
 * 	- Resets the movement builder;
 */

cnode_state_t mcontroller_initialise_calculation(mcontroller_t *const mcontroller) {

	/*
	 * Builder reset;
	 */

	//Cache the builder;
	movement_builder_t *const builder = &mcontroller->movement_builder;

	//Reset the distance alteration flag;
	builder->distances_altered = false;

	//Update the current movement;
	builder->movement =;//TODO CREATE THE QUEUE AND PEAK THE INPUT ELEMENT

	//Reset the time interval;
	*builder->interval = (time_interval_t) {
		.min = 0,
		.max = 0,
		.max_exists = false,
		.singleton = false,
	};


	/*
	 * position computer reset;
	 */

	//Reset the position computer;
	(*(mcontroller->position_computer_reset))(mcontroller->position_computer);


	//Always succeed;
	return STAGE_COMPLETE;

}


/*
 * mcontroller_compute_distances : calls the distance computation function;
 */

cnode_state_t mcontroller_compute_distances(mcontroller_t *const mcontroller) {

	//TODO EVENTUALLY PRECOMPUTE AT INIT;

	//Compute the position and return the status;
	distances_computer_compute(&mcontroller->distances_computer,

		//Pass the geometry;
							   &mcontroller->machine_constants,

		//Pass the current machine state;
							   machines_states_current_state(&mcontroller->states),

		//Pass the controller part of the machine state;
							   machines_states_candidate_state(&mcontroller->states)->controller_state,

		//Pass the final distances array;
							   mcontroller->movement_builder.movement->actuation_distances);

}


/*
 * mcontroller_compute_distances : calls the distance computation function;
 */

cnode_state_t mcontroller_compute_duration_interval(mcontroller_t *const mcontroller) {

	//Cache the dimension;
	const uint8_t dimension = mcontroller->machine_constants.dimension;

	//Cache the movement builder's final_interval array;
	time_interval_t *const intervals = mcontroller->movement_builder.interval_array;

	//Cache the final final_interval;
	time_interval_t *const final_interval = &mcontroller->movement_builder.duration_interval;

	//Cache the speed constraints array;
	actuation_speed_constraint_t *const constraints = mcontroller->actuation_speed_constraints;


	//Cache the movement builder's final_interval array;
	time_interval_t *interval_p = intervals;

	//Cache the speed constraints array;
	actuation_speed_constraint_t *constraint_p = constraints;


	//For each actuator :
	for (uint8_t actuator = 0; actuator < dimension; actuator++) {

		//TODO MULTITHREAD
		//Compute the related duration final_interval;
		_mcontroller_compute_duration_interval(constraint_p++, interval_p++);

	}

	//Merge duration windows to obtain the minimal acceptable one;
	mcontroller_merge_intervals(intervals, final_interval);

	//If the minimal acceptable final_interval is empty (min is superior to max);
	if (final_interval->empty) {

		//Correct the interval;
		mcontroller_correct_interval(final_interval);

		//Initialise the time bounds pointer;
		interval_p = intervals;

		//For each actuator :
		for (uint8_t actuator = 0; actuator < dimension; actuator++) {

			//If the final duration is out of its duration window:
			if (interval_p) {

				//Initializer for distance re-computation data;


				//Determine the distance that matches the final duration;
				mcontroller_recompute_distance();//TODO

			}

			//Update the interval pointer;
			interval_p++;

		}

	}

	//Finally, we

}


/*
 * _mcontroller_compute_duration_interval : computes the duration interval for a single actuator;
 *
 * 	Requires the movement builder, and an actuator index;
 */

void _mcontroller_compute_duration_interval(actuation_speed_constraint_t *constraint, time_interval_t *interval) {

	//TODO


}


void mcontroller_merge_intervals(const time_interval_t *intervals, time_interval_t *const merges_interval) {

	//TODO

}


void mcontroller_correct_interval(time_interval_t *const final_interval) {

	//Set the final duration as the average of min and max;
	float final_duration = (final_interval->min + final_interval->max) / (float) 2;

	//Update the interval;
	*final_interval = {
		.min = final_duration,
		.max = final_duration,
		.max_exists = true,
		.singleton = true,
		.empty = true,
	};

}

