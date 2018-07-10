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

#include "machine_core.h"

#include "kernel/computation/staged_execution.h"
#include "movement.h"
#include "machine.h"


//---------------------------------------------------- Nodes headers ---------------------------------------------------


/*
 * distances_computation : Node 1, activated by 0, priority 1;
 *
 * 	Computes the new movement's distances, given the current state;
 *
 * 	This function is taken in charge by the machine controller, that will secure the eventual change of computer;
 *
 * 	The args structure wraps args that it wills transmit
 */

typedef struct {

	//The movement controller's instance;
	machine_controller_t *controller;

	//Distances_computation_args, as defined by the movement controller;
	distances_computation_args args;

} mcore_distances_computation_args;


//-----------------------------------------------

/*
 * Duration intervals computation : Node 2, activated by 1, priority 2;
 *
 * 	Computes the duration interval for one axis;
 *
 * 	This function is managed by the actuation physics module, no wrapping is required;
 */


//-----------------------------------------------

/*
 * interval_merge : Node 3, activated by 0, priority 3
 *
 * 	Will determine the best admissible interval, given a set of duration intervals;
 *
 * 	If authorised, will activate the distance correction node for all required actuators;
 *
 * 	Will activate the next position computation;
 *
 * 	If authorised, will activate the geometry inversion, to keep control positions relevant;
 *
 * 	Outputs : Node 4;
 * 	Outputs : Node 5;
 *
 * 	This function requires access to a lot of data, so it would be a waste of processing time to copy and store;
 * 	It just takes the whole structure, and as its behaviour is the same for all types of
 * 	movements, it will be as secure as I can do;
 */

void interval_merge(machine_core_t *mcontroller);


//-----------------------------------------------

/*
 * distance_correction : Node 4, activated by 3, priority 4;
 *
 * 	Will determine the closest distance from the original distance, that matches actuation constraints;
 *
 * 	This function is managed by the actuation physics module, no wrapping is required;
 */


//-----------------------------------------------

/*
 * next_positions_computation : Node 5, activated by 3, priority 5;
 *
 * 	Will determine next actuation positions, given the current state, and the distances array.
 */

typedef struct {

	//The machine's dimension, constant;
	const uint8_t dimension;

	//Current actuation positions, constant;
	const int32_t *const positions;

	//Corrected movement distances, constant;
	const int16_t *const distances;

	//Destinations array. Mutable;
	int32_t *const destinations;

} next_position_computation_args;

void next_position_computation(next_position_computation_args *args);


//-----------------------------------------------

/*
 * geometry inversion : Node 6, activated by 3, priority 6;
 *
 *	Will invert the geometry, to get control positions from actuation positions.
 *
 * 	This function is managed by the geometry module, no wrapping is required;
 */


//-----------------------------------------------

/*
 * controller_builder_computation : Node 7, activated by 0, priority 7;
 *
 * 	As computation steps are ordered, and so, do not support parallel execution, this function is delegated to
 * 	the machine controller, that will call each one of them in order;
 */

typedef struct {

	//The movement controller's instance;
	machine_controller_t *controller;

	//builder_computation_args, as defined by the movement controller;
	builder_computation_args args;

} mcore_builder_computation_args;


//-----------------------------------------------

/*
 * duration_computation : Node 8, activated by TODO, priority TODO;
 *
 * 	As computation steps are ordered, and so, do not support parallel execution, this function is delegated to
 * 	the machine controller, that will call each window correction step in order;
 */

typedef struct {

	//The movement controller's instance;
	machine_controller_t *controller;

	//duration_computation_args, as defined by the movement controller;
	duration_computation_args args;

} mcore_duration_computation_args;


//-----------------------------------------------

/*
 * Controller state computation : Node 9, activated by TODO, priority TODO;
 *
 * 	As computation steps are ordered, and so, do not support parallel execution, this function is delegated to
 * 	the machine controller, that will call each one of them in order;
 */

struct {

	//The movement controller's instance;
	machine_controller_t *controller;

	//state_computation_args, as defined by the movement controller;
	state_computation_args args;

} mcore_state_computation_args;



//--------------------------------------------------- Private headers --------------------------------------------------

//Regroup computed intervals in a single one;
time_interval_t mcontroller_regroup_intervals(uint8_t dimension, const time_interval_t *intervals);

//Arbitrarily choose a duration if duration windows didn't match;
void mcontroller_choose_duration(time_interval_t *final_interval, float previous_time);

//Activate distance corrections for relevant actuators;
void mcontroller_activate_corrections(uint8_t dimension, float time, const time_interval_t *intervals);


//--------------------------------------------------- Implementations --------------------------------------------------

/*
 * interval_merge : this functions will regroup all duration intervals in a single one, and if required,
 * 	choose a time, and activate distances corrections for relevant actuators;
 */

void interval_merge(machine_core_t *mcontroller) {

	//Cache the dimension;
	const uint8_t dimension = mcontroller->machine_constants.dimension;

	//Cache the movement builder;
	movement_buinlder_t *const builder = &mcontroller->movement_builder;

	//Cache the interval array pointer;
	const time_interval_t *const intervals = builder->interval_array;

	//Regroup intervals in a single one;
	time_interval_t final_interval = mcontroller_regroup_intervals(dimension, intervals);

	//If the required movement didn't match with actuation physical limitations :
	if (final_interval.empty) {

		//Choose arbitrarily a duration, eventually the previous movement's duration;
		mcontroller_choose_duration(&final_interval, builder->duration_interval.min);

		//Activate distance correction for relevant actuators;
		mcontroller_activate_corrections(dimension, final_interval.min, intervals);

	}

	//Save the time interval;
	builder->duration_interval = final_interval;

}


/*
 * mcontroller_regroup_intervals : this function returns an interval that :
 * 	- has its max as the minimum of all intervals maximum bounds;
 * 	- has its min as the maximum of all intervals minimum bounds;
 * 	- is empty if all movements are empty;
 * 	- is valid if all movements are valid, and min <= max;
 */

time_interval_t mcontroller_regroup_intervals(const uint8_t dimension, const time_interval_t *const intervals) {

	//Declare the final interval; will be saved at the end;
	time_interval_t final_interval = {

		//Largest width;
		.max_exists = false,
		.max = 0,
		.min = 0,

		//Initialised empty, will be cleared as soon as a non empty interval is found;
		.empty = true,

		//Initialised valid, will be cleared if a non valid interval is found;
		.valid = true,

	};

	//We will determine the minimum of all max bounds, and the maximum of all min bounds;

	//For each interval except the first (done one line earlier):
	for (uint8_t actuator_index = 0; actuator_index < dimension; actuator_index++) {

		//Cache the current interval pointer;
		const time_interval_t *const interval = intervals + actuator_index;

		//Propagate the invalidity;
		final_interval.valid = final_interval.valid && interval->valid;

		//If the interval is empty, it is not taken into account in the bounds computation;

		//If the actuator's duration interval is not empty :
		if (!interval->empty) {

			//Mark the final interval not empty;
			final_interval.empty = false;

			//The minimal time always exists;
			float min = interval->min;

			//If the interval's is superior to the computed minimum :
			if (min > final_interval.min) {

				//Update the minimum bound;
				final_interval.min = min;

			}

			//If the interval's maximal time exists :
			if (interval->max_exists) {

				//Cache it;
				float max = interval->max;

				//If the final interval's max bound exists :
				if (final_interval.max_exists) {

					//If it is inferior to the computed maximum :
					if (max < final_interval.max) {

						//Update the minimum bound;
						final_interval.max = max;

					}

				} else {
					//If the final interval's max doesn't exist :

					//Update it to the interval's one, and mark the max bound existing;
					final_interval.max_exists = true;
					final_interval.max = max;

				}

			}

		}
	}

	//Update the validity flag to consider also bounds crossing;
	final_interval.valid |= (final_interval.min > final_interval.max);

	//Return the computed interval;
	return final_interval;

}


/*
 * mcontroller_choose_duration : this function will choose an arbitrary duration, if the final interval is invalid;
 *
 * 	Depending on the interval, it will select the previous time, or the average of min and max;
 *
 * 	After choosing the time, it will re-initialise the interval to a safe, valid, singleton;
 */

void mcontroller_choose_duration(time_interval_t *const final_interval, const float previous_time) {

	//We must choose an arbitrary time;
	float time;

	//If a time interval is not provided, or if no actuator could provide a possible duration window (rare):
	if (!final_interval->max_exists) {

		//The movement will have the same duration as the previous one;
		time = previous_time;

	} else {
		//If a time interval exist, choose the average btw min and max bound;

		//Arbitrary choose the average of min and max;
		time = (final_interval->max + final_interval->min) / (float) 2;

	}

	//Save the time and update flags;
	*final_interval = (time_interval_t) {
		.max_exists = true,
		.max = time,
		.min = time,
		.valid = true,
		.empty = false,
	};

}


/*
 * mcontroller_activate_corrections : will traverse all time intervals, and activate the distance correction for the
 * 	invalid ones, or for those that do not contain the new time;
 */

void mcontroller_activate_corrections(const uint8_t dimension, const float time, const time_interval_t *const intervals) {

	//For each actuator :
	for (uint8_t actuator_index = 0; actuator_index < dimension; actuator_index++) {

		//Cache the current interval pointer;
		const time_interval_t *interval = intervals + actuator_index;

		//If the interval is empty, or if the final time is out of it, a correction is required;
		if ((!interval->valid) || (time < interval->min) || ((interval->max_exists) && (time > interval->max))) {

			//TODO ACTIVATE CORRCTION;

		}

	}

}


/*
 * next_position_computation : fills array containing the next position, knowing the current one, and distances;
 */

void next_position_computation(next_position_computation_args *args) {

	//Cache arrays;
	const int32_t *postions = args->positions;
	const int16_t *distances = args->distances;
	int32_t *dests = args->destinations;

	//For each actuator :
	for (uint8_t actuator_index = args->dimension; actuator_index--;) {

		//Set the destination to the sum of the current position and converted distance; Update pointers btw;
		*(dests++) = *(postions++) + (int32_t)*(distances++);

	}

}


/*
 * mcontroller_compute_duration_interval : computes the duration interval for a single actuator;
 *
 * 	Entry :
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

