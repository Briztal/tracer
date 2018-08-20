/*
  machine_core.c Part of TRACER

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
#include <data_structures/containers/non_concurrent/circular_buffer.h>
#include "machine_core.h"
#include "machine_states.h"
#include "movement_builder.h"

//---------------------------------------------------- Nodes headers ---------------------------------------------------


/*
 * distances_computation : Node 1, activated by 0, priority 1;
 *
 * 	Computes the new movement's distances, given the current state;
 *
 * 	This function is taken in charge by the machine controller, no wrapping is required;
 */

//-----------------------------------------------

/*
 * Duration intervals comput : Node 2, activated by 0, priority 2;
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
 * 	Will activate the next position comput;
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

void interval_merge(const machine_core_t *mcontroller);


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
 * next_positions_computation : Node 5, activated by 0, priority 5;
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

void next_position_computation(const next_position_computation_args *args);


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
 * 	As comput steps are ordered, and so, do not support parallel execution, this function is delegated to
 * 	the machine controller, that will call each one of them in order;
 */

typedef struct {

	//The movement controller's instance;
	machine_controller_t *const controller;

	//builder_computation_args, as defined by the movement controller;
	const builder_computation_args args;

} mcore_builder_computation_args;


//-----------------------------------------------

/*
 * duration_computation : Node 8, activated by 0, priority 8;
 *
 * 	As comput steps are ordered, and so, do not support parallel execution, this function is delegated to
 * 	the machine controller, that will call each window correction step in order;
 */

typedef struct {

	//The movement controller's instance;
	machine_controller_t *const controller;

	//duration_computation_args, as defined by the movement controller;
	const duration_computation_args args;

} mcore_duration_computation_args;


//-----------------------------------------------

/*
 * Controller state comput : Node 9, activated by 0, priority 9;
 *
 * 	As comput steps are ordered, and so, do not support parallel execution, this function is delegated to
 * 	the machine controller, that will call each one of them in order;
 */

struct {

	//The movement controller's instance;
	const machine_controller_t *const controller;

	//state_computation_args, as defined by the movement controller;
	state_computation_args args;

} mcore_state_computation_args;



//-----------------------------------------------

/*
 * State validation : Node 9, activated by 0, priority 9;
 *
 * 	As comput steps are ordered, and so, do not support parallel execution, this function is delegated to
 * 	the machine controller, that will call each one of them in order;
 */

struct {

	//The movement controller's instance;
	const machine_controller_t *const controller;

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
 * machine_core_create : creates a machine core from machine constants, a controller,
 * and an array of actuator physics managers;
 */

machine_core_t *machine_core_create(const machine_constants_t *const machine_constants,
									machine_controller_t *const controller,
									const uint8_t nb_actuator_physics,
									const actuator_physics_t *const actuators_physics_src,
									const size_t movement_buffer_size) {

	//Cache the dimension;
	uint8_t dimension = machine_constants->dimension;

	//If the number of actuator physics managers is different from the dimension :
	if (dimension != nb_actuator_physics) {

		//Error, there must be a physics manager for each actuator, and one actuator per axis;
		kernel_error("machine_core.c : machine_core_create : the number of actuator physics controllers is "
						 "not equal to the dimension.");

	}

	//Query two initialised controller states;
	void *controller_state_0 = machine_controller_create_state(controller);
	void *controller_state_1 = machine_controller_create_state(controller);

	//Query one initialised controller builder;
	void *controller_builder = machine_controller_create_builder(controller);

	//Duplicate the actuators physics controllers array;
	const actuator_physics_t *const actuators_physics =
		kernel_malloc_copy(sizeof(actuator_physics_t) * nb_actuator_physics, actuators_physics_src);


	//Create an initializer for the machine core;
	machine_core_t core_init = {

		//Copy the machine constants;
		.machine_constants = *machine_constants,

		//Do not initialise the states manager for instance, will be done right after;
		.states = {},

		//Do not initialise the movement builder for instance, will be done right after;
		.movement_builder = {},

		//Transfer the ownership of the machine controller;
		.controller = controller,

		//Transfer the ownership of the actuators physics controllers array;
		.actuation_physics = actuators_physics,


		//---------------------------- Movement buffer ----------------------------

		//The movement buffer, owned, mutable;
		.movement_buffer = EMPTY_CBUFFER(movement_t, movement_buffer_size),

	};

	//Allocate and initialise the machine core;
	machine_core_t *const machine_core = kernel_malloc_copy(sizeof(machine_core_t), &core_init);

	//Resize the movement buffer to its maximal size;
	cbuffer_resize(&machine_core->movement_buffer, movement_buffer_size);

	//Initialise the states manager;
	machine_states_initialise(&machine_core->states, dimension, controller_state_0, controller_state_1);

	//Initialise the movement builder;
	movement_builder_initialise(&machine_core->movement_builder, dimension, controller_builder);

	//TODO INITIALISE THE COMPUTATION NETWORK;

	//Return the initialised machine core;
	return machine_core;

}


/*
 * machine_core_delete : deletes the provided machine core;
 *
 * 	Deletes controller states and builder, frees the actuation physics controller array,
 * 	and calls deletion for the movement buffer, the controller,
 * 	the states manager, and the movement builder;
 *
 * 	Finally, it frees the machine core structure itself;
 */

void machine_core_delete(machine_core_t *machine_core) {

	//Cache the machine controller;
	machine_controller_t *controller = machine_core->controller;

	//Delete controller states;
	machine_controller_delete_state(controller, machine_core->states.s0.controller_state);
	machine_controller_delete_state(controller, machine_core->states.s1.controller_state);

	//Delete controller builder;
	machine_controller_delete_builder(controller, machine_core->movement_builder.controller_builder);

	//Free the actuation physics controller array;
	kernel_free((void *) machine_core->actuation_physics);

	//Delete the movement buffer;
	cbuffer_delete(&machine_core->movement_buffer);

	//Delete the controller;
	machine_controller_delete(machine_core->controller);

	//Delete the states manager;
	machine_states_delete(&machine_core->states);

	//Delete the movement builder;
	movement_builder_delete(&machine_core->movement_builder);

}

//Execute some stages in the movement comput;
bool machine_core_compute_movements(machine_core_t *const core, uint8_t nb_stages) {

	//If the queue is already full :
	if (!((volatile size_t) core->movement_buffer.nb_spaces)) {

		//No comput to do until the next movement discard;
		return false;

	}

	//Execute the required number of computations;
	while (nb_stages--) {
		cnetwork_execute(core->movement_computation);
	}


	//If the queue is already full :
	if (!((volatile size_t) core->movement_buffer.nb_spaces)) {

		//No comput to do until the next movement discard;
		return false;

	}



}


/*
 * machine_core_available_movements : returns the number of available movements in the buffer;
 */

size_t machine_core_available_movements(machine_core_t *core) {

	//Return the number of available movements;
	return core->movement_buffer.nb_elements;

}


/*
 * read_movement : returns the pointer to the first available movement in the buffer;
 */

movement_t *read_movement(machine_core_t *core) {

	//Get the output pointer;
	return cbuffer_read_output(&core->movement_buffer, 0);

}


/*
 * discard_movement : discards the first movement of the buffer;
 */

void discard_movement(machine_core_t *core) {

	//Discard the output element;
	cbuffer_discard_output(&core->movement_buffer);

}


//------------------------------------------------ Nodes implementation ------------------------------------------------

/*
 * interval_merge : this functions will regroup all duration intervals in a single one, and if required,
 * 	choose a time, and activate distances corrections for relevant actuators;
 */

void interval_merge(const machine_core_t *const mcontroller) {

	//Cache the dimension;
	const uint8_t dimension = mcontroller->machine_constants.dimension;

	//Cache the movement builder;
	movement_builder_t *const builder = &mcontroller->movement_builder;

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

		//If the interval is empty, it is not taken into account in the bounds comput;

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

void
mcontroller_activate_corrections(const uint8_t dimension, const float time, const time_interval_t *const intervals) {

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

void next_position_computation(const next_position_computation_args *const args) {

	//Cache arrays;
	const int32_t *postions = args->positions;
	const int16_t *distances = args->distances;
	int32_t *dests = args->destinations;

	//For each actuator :
	for (uint8_t actuator_index = args->dimension; actuator_index--;) {

		//Set the destination to the sum of the current position and converted distance; Update pointers btw;
		*(dests++) = *(postions++) + (int32_t) *(distances++);

	}

}


