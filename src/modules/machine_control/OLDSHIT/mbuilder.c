//
// Created by root on 6/24/18.

#include "mbuilder.h"
#include "trajectory.h"
#include "movement.h"

#include <kernel/kernel.h>

#include <string.h>


/*
 * mbuilder_init : initialises the previously allocated mbuilder;
 *
 * 	Allocates all arrays, and initialises the struct;
 */

void mbuilder_init(mbuilder_t *builder, size_t dimension) {

	//Allocate the destination control position array;
	float *control_destination = kernel_malloc(sizeof(float) * dimension);

	//Allocate t0 and t1 arrays for control and actuation positions;
	float *actuation_t0 = kernel_malloc(sizeof(float) * dimension);
	float *actuation_t1 = kernel_malloc(sizeof(float) * dimension);

	//Allocate the actuation distances array;
	float *actuation_distances = kernel_malloc(sizeof(float) * dimension);

	//Initializer for the movement builder;
	mbuilder_t init = {

		//Default value for the index monitor;
		.index_monitor = {

			//Default;
			.index = 0,
			.increment = 0,
			.increasing_index = true,
			.limit_index = 0,
			.index_candidate = 0,

			//The limit index is selected by default;
			.limit_index_reached = true,

		},

		//Save the allocated control array;
		.control_destination = control_destination,

		//Save actuation arrays;
		.actuation_t0 = actuation_t0,
		.actuation_t1 = actuation_t1,

		//Set the current position to be represented by actuation_t0;
		.current_is_t0 = true,

		//Save the actuation distances array;
		.actuation_distances = actuation_distances,

	};

	//Initialise the builder;
	memcpy(builder, &init, sizeof(mbuilder_t));

}


/*
 * mbuilder_delete : deletes all array from the provided mbuilder;
 */

void mbuilder_delete(mbuilder_t *mbuilder) {

	//Free all arrays;
	kernel_free(mbuilder->control_destination);
	kernel_free(mbuilder->actuation_t0);
	kernel_free(mbuilder->actuation_t1);
	kernel_free(mbuilder->actuation_distances);

}


/*
 * mbuilder_switch : Changes the value of the flag in order to switch current/previous representations;
 */

void mbuilder_switch_actuation_arrays(mbuilder_t *mbuilder) {

	//Change the value of the flag;
	bool t0 = mbuilder->current_is_t0;
	mbuilder->current_is_t0 = !t0;

}


//--------------------------------------------------- Building steps --------------------------------------------------

/*
 * movement_compute_new : computes a new position;
 *
 * 	It makes an increment check to verify if it must select the final index value (complete the trajectory), and
 * 	return true if it does;
 *
 */

bool movement_compute_index(index_monitor_t *const index_monitor) {

	//Cache all data we need, in order of declaration;
	const float index = index_monitor->index;
	const float increment = index_monitor->increment;
	const bool increasing_index = index_monitor->increasing_index;
	const float limit = index_monitor->limit_index;


	//------------------Index computing-----------------------------

	//Get the new index candidate;
	float index_candidate = index + increment;

	//Get the next index candidate;
	const float next_index_candidate = index_candidate + increment;

	//Determine if the next index will be inferior to the limit;
	const bool next_inferior = next_index_candidate < limit;

	//Declare a flag, asserting that the limit index selected;
	bool limit_index_selected = false;

	//If the index is close of less than one increment of its limit (a xor determines this assertion) :
	if (increasing_index ^ next_inferior) {

		//Set the index to the limit;
		index_candidate = limit;

		//Mark the limit index selected;
		limit_index_selected = true;

	}

	//Save the index candidate
	index_monitor->index_candidate = index_candidate;

	//Save the limit flag;
	index_monitor->limit_index_reached = limit_index_selected;

}


void mbuilder_compute_destination(mbuilder_t *const builder, const curve_t *const curve) {


	//Evaluate the curve at the current index, and store the result in the control positions array;
	curve_evaluate(curve, builder->index_monitor.index_candidate, builder->control_destination);

}

void mbuilder_convert_position(mbuilder_t *const builder, const geometry_t *const geometry) {

	//Declare a pointer to the destination;
	float *destination;

	//Assign the pointer;
	mbuilder_get_actuation_destination(builder, &destination);

	//Convert the high level position into the assigned array;
	geometry_convert(geometry, builder->control_destination, destination);


}


/*
 * mbuilder_compute_distances : computes actuation distances for the provided movement, and update current positions
 * in the mbuilder;
 *
 * It also returns the maximal distance;
 */

uint16_t mbuilder_compute_distances(const uint8_t dimension, mbuilder_t *const builder, movement_t *const movement) {

	//Declare actuation current pointers;
	float *destination;
	const float *current;

	//Assign pointers;
	mbuilder_get_actuation_positions(builder, &current, &destination);

	//Cache the float distances array;
	float *float_distances = builder->actuation_distances;

	//Cache the integer distances array;
	uint16_t *integer_dists = movement->actuation_distances;

	//Cache variable, to avoid pointer access.
	float maximal_float_distance = 0;
	sig_t direction_signature = 0;

	//Cache the current bit of the signature;
	sig_t signature_bit = 1;

	//For each actuator :
	for (uint8_t axis_index = 0; axis_index < dimension; axis_index++) {

		//Cache destination and current
		float d = destination[axis_index], p = current[axis_index];

		//get the real distance
		float float_distance = d - p;

		uint8_t integer_distance;

		//Compute absolute distance;
		if (float_distance < 0) {
			float_distance = -float_distance;
			direction_signature |= signature_bit;
			integer_distance = (uint8_t) ((int32_t) p - (int32_t) d);
		} else {

			integer_distance = (uint8_t) ((int32_t) d - (int32_t) p);
		}

		//Update the current bit to the next one;
		signature_bit <<= 1;

		//Save computed distances;
		float_distances[axis_index] = float_distance;
		integer_dists[axis_index] = integer_distance;


		//If the maximal float distance is lower than the current :
		if (float_distance > maximal_float_distance) {

			//Update integer and float maximal distances;
			maximal_float_distance = float_distance;

		}

	}

	//Save the signature
	movement->direction_signature = direction_signature;

	//Save the maximal float distance;
	builder->maximal_distance = maximal_float_distance;

}


bool mbuilder_validate_distances(mbuilder_t *builder, movement_t *movement, dbounds_t bounds) {

	//Cache the maximal distance;
	const float max_distance = builder->maximal_distance;

	//Cache the distance target;
	const uint16_t distance_target = bounds.target;

	//Determine if the distance goes over its upper limit;
	bool up_error = (max_distance >= bounds.max);

	//Increment adjustment according to the targetVector
	if ((uint16_t) max_distance != distance_target) {

		//get the distance ratio
		float ratio = (float) distance_target / max_distance;

		//Update the increment with the ratio;
		builder->index_monitor.increment *= ratio;

	}

	//Cache the trajectory completion flag;
	bool trajectory_complete = builder->index_monitor.limit_index_reached;

	//If the upper limit, and the trajectory is not complete, invalidate;
	if (up_error && (!trajectory_complete)) {
		return false;
	}

	//If the maximal distance is below the lower limit :
	if (max_distance <= bounds.min) {

		//If the trajectory is complete :
		if (trajectory_complete) {

			//Error, not supposed to happen, the limit overlap test should have prevented this;
			kernel_error("mbuilder.c : mbuilder_validate_distances : last movement doesn't have enough steps."
							 "The limit index reach test must be enlarged to two increments;");

		}

		//Invalidate the movement;
		return false;

	}

	//The movement is valid;

	//Switch actuation arrays;
	mbuilder_switch_actuation_arrays(builder);

	//Update the index;
	builder->index_monitor.index = builder->index_monitor.index_candidate;

	return true;
}


void mbuilder_compute_kinematics(mbuilder_t *builder, movement_t *movement) {

	//Call the Kinematics Manager, to fill the kernel-reserved part of the data.
	Kinematics::initialise_movement_builder(builder);

	//Update the kinematics data
	Kinematics::accept_sub_movement(movement);

}