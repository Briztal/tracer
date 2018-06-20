/*
  movement.c Part of TRACER

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

#include "movement.h"
#include "trajectory.h"

#include <kernel/kernel.h>
#include <string.h>

//-------------------------------------------------- Private headers ---------------------------------------------------

//Determine increments;
bool movement_determine_increments(const tcontroller_t *trajectory, const curve_t *curve,
								   trajectory_increments_t *increments);


//Extract the increment for a particular point;
float movement_extract_increment(const tcontroller_t *trajectory, const curve_t *curve, float point, float increment);



//Get the maximal distance between two positions;
uint32_t get_max_dist(uint8_t dimension, const float *p0, const float *p1);


//--------------------------------------------------- Implementation----------------------------------------------------

/*
 * movement_init : initialises a movement from its primary data and the trajectory's requirements,
 *	and add it to the trajectory.
 *
 * 	If the movement is invalid, the movement is deleted;
 */

void movement_init(movement_t *movement, const uint8_t dimension, const curve_t *const curve,
				   tcontroller_t *trajectory) {

	//Initialise the increments struct;
	trajectory_increments_t increments {};

	//Determine increments; Returns false in case of a micro movement;
	bool b = movement_determine_increments(trajectory, curve, &increments);

	//If the movement is a micro movement :
	if (!b) {

		//Delete the movement;
		kernel_free(movement);

	}

	//Initialise jerk position;
	int32_t *jerk_position = kernel_malloc(((size_t) dimension) * sizeof(int32_t));

	//Initialise jerk ratios;
	float *jerk_ratios = kernel_malloc(((size_t) dimension) * sizeof(float));

	//Create the initializer;
	movement_t init = {

		//An empty linked element;
		.link = EMPTY_LINKED_ELEMENT(),

		//The movement's dimension;
		.dimension = dimension,

		//The movement's equation;
		.curve = *curve,

		//Increments data;
		.increments = increments,

		//Jerk structure;
		.jerk_data = {

			//No jerk point by default;
			.jerk_point = false,

			//Not initialised;
			.jerk_position = jerk_position,

			//Not initialised;
			.ending_jerk_ratios = jerk_ratios,

		},

	};

	//Initialise the movement;
	memcpy(movement, &init, sizeof(init));

	//Enqueue the movement to the trajectory;
	tcontroller_enqueue(trajectory, movement);

}


/*
 * movement_delete : deletes previously allocated data and frees the movement struct;
 */

void movement_delete(movement_t *movement) {

	//Call the implementation deletion;
	(*(movement->deletion_function))(movement);

	//Free the jerk position;
	kernel_free(movement->jerk_data.jerk_position);

	//Free the jerk ratios array;
	kernel_free(movement->jerk_data.ending_jerk_ratios);

	//Free the movement structure;
	kernel_free(movement);

}


//----------------------------------------------------- Increments -----------------------------------------------------

/*
 * determine_increments : this function is the only public method of the class.
 *
 * It computes the increment for initial and final positions, for the given movement
 *
 * It handles the micro movement case.
 */

bool movement_determine_increments(const tcontroller_t *trajectory, const curve_t *curve,
								   trajectory_increments_t *increments) {

	//Cache indices
	float initial_index = curve->initial_index, final_index = curve->final_index;

	//Cache increments;
	float initial_incr = 0, final_incr = 0;

	//A flag determining if the movement is invalid;
	bool invalid;

	//If the index is ascending :
	if (initial_index < final_index) {

		//Determine the initial increment;
		initial_incr = movement_extract_increment(trajectory, curve, initial_index, 1);
		final_incr = movement_extract_increment(trajectory, curve, final_index, -1);

		//If the two basic movements overlap each other, the movement is a micro movement (invalid)
		invalid = initial_index + initial_incr > final_index + final_incr;


	} else {

		//If the index is descending :
		initial_incr = movement_extract_increment(trajectory, curve, initial_index, -1);
		final_incr = movement_extract_increment(trajectory, curve, final_index, 1);

		//If the two basic movements overlap each other, the movement is a micro movement (invalid);
		invalid = initial_index + initial_incr < final_index + final_incr;

	}

	//If the movement has been marked invalid :
	if (invalid) {

		//Fail;
		return false;

	}

	//Save cached increments;
	increments->initial = initial_incr;
	increments->final = final_incr;

	//Complete
	return true;
}


/*
 * extract_increment : this function receives
 * 		- dimension : the movement's dimension;
 * 		- distance_target : the targeted distance for increments;
 *      - equation : a trajectory function
 *      - point : a float, of the dimension of get_function's input.
 *      - increment : a distance from point, of the same dimension than point
 *      - distance_target : a distance in the trajectory_control coordinate system.
 *
 *  With these inputs, it extracts a value of increment that verifies the property below.
 *
 *  "The maximum step_distances on all trajectory_control axis between the images of point and point+increment through the
 *      trajectory function is target_distance"
 *
 *  Literally, this function is called during the scheduling of a movement, when the increment variable
 *      must be determined.
 *
 */

float movement_extract_increment(const tcontroller_t *const trajectory, const curve_t *const curve, float point, float increment) {

	//Cache the dimension;
	const uint8_t dimension = trajectory->(trajectory);

	//Cache the distance target;
	const uint16_t distance_target = trajectory->actuation->distance_target;

	//Arrays initialisation
	float initial_positions[dimension], positions[dimension];

	//Cache the geometry;
	geometry_t *geometry = trajectory->geometry;

	//Get initial positions
	curve_evaluate_convert(curve, dimension, geometry, point, initial_positions);

	//cache float version of distance_target
	float ftarget = (float) distance_target;


	//Loop to execute to correct the increment;
	do {

		//Get the candidate actuation position;
		curve_evaluate_convert(curve, dimension, geometry, point + increment, positions);

		//Get the distance between the initial distance and the candidate position.
		uint32_t d = get_max_dist(dimension, initial_positions, positions);

		//Catch the case where d becomes 0.
		if (!d) d = 1;

		if (d == distance_target) {

			//return the increment that gave the correct increment;
			return increment;

		}

		//Update the increment
		increment *= ftarget / (float) d;


		//While the increment has not been reached;
	} while(1);

}


/*
 * get_max_dist : this function gets the maximal distance between the two provided positions, p0 and p1.
 *
 *  For each axis, it determines the real distance (float), and determines the absolute integer distance.
 */

uint32_t get_max_dist(uint8_t dimension, const float *p0, const float *p1) {

	//initialise_hardware the maimum distance
	uint32_t max_dist = 0;

	//for each trajectory_control
	while (dimension--) {

		//get the algebraic distance
		int32_t dist = (int32_t) p1[dimension] - (int32_t) p0[dimension];

		//obtain the absolute distance
		if (dist < 0) {
			dist = -dist;
		}

		//cast to unsigned
		uint32_t distance = (uint32_t) dist;

		//update max_dist
		if (distance > max_dist) {
			max_dist = distance;
		}

	}

	//return the maximum distance.
	return max_dist;

}



