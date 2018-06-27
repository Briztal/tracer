/*
  trajectory.c Part of TRACER

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

#include "trajectory.h"
#include "movement.h"
#include "tcontroller.h"

#include <kernel/kernel.h>

#include <string.h>

//-------------------------------------------------- Private headers ---------------------------------------------------

//Determine increments;
bool curve_determine_increments(const geometric_base_t *geometric_base, trajectory_increments_t *increments);

//Extract the increment for a particular point;
float curve_extract_increment(const geometric_base_t *geometric_base, float point, float increment);

//Get the maximal distance between two positions;
float get_max_dist(uint8_t dimension, const float *p0, const float *p1);


//--------------------------------------------------- Curves ----------------------------------------------------

//Evaluate the curve in one point and translate the result in control coordinates;
void curve_evaluate_convert(const geometric_base_t *const geometric_base, float index, float *dest) {

	//Cache the curve pointer;
	const curve_t *const curve = geometric_base->curve;

	//The temporary control position;
	float temp[curve->dimension];

	//Evaluate the curve and store the result in the temporary array;
	curve_evaluate(curve, index, temp);

	//Translate the geometry;
	geometry_convert(geometric_base->geometry, temp, dest);

}

//Evaluate the curve in one point and translate the result in control coordinates; Uses a provided temporary array;
void curve_evaluate_convert_tmp_provided(const geometric_base_t *const geometric_base, float *const tmp, float index, float *dest) {

	//Cache the curve pointer;
	const curve_t *const curve = geometric_base->curve;

	//Evaluate the curve and store the result in the temporary array;
	curve_evaluate(curve, index, tmp);

	//Translate the geometry;
	geometry_convert(geometric_base->geometry, tmp, dest);

}

//---------------------------------------------------- Trajectories ----------------------------------------------------

/*
 * trajectory_init : initialises a trajectory from its primary data and the trajectory's requirements,
 *	and add it to the trajectory list.
 *
 * 	If the trajectory is invalid, the trajectory is deleted;
 */

void trajectory_init(trajectory_t *const trajectory, const curve_t *const curve, bool affine_curve,
					 actuation_t *const actuation) {

	//Cache the dimension;
	uint8_t dimension = curve->dimension;

	//Cache the trajectory controller; Its access is critical, so we won't modify it or pass it;
	const tcontroller_t *const controller = actuation->tcontroller;

	//Declare a geometric base;
	const geometric_base_t geometric_base = {
		.dimension = dimension,
		.geometry  = controller->geometry,
		.curve = curve,
		.dbounds = controller->distance_bounds,
	};


	//Initialise the increments struct;
	trajectory_increments_t increments = {};

	//Determine increments; Returns false in case of a micro trajectory;
	bool b = curve_determine_increments(&geometric_base,  &increments);

	//If the trajectory is a micro trajectory :
	if (!b) {

		//Delete the trajectory;
		kernel_free(trajectory);

	}

	//If the geometry is irregular, no optimisation is allowed;
	if (!geometric_base.geometry->regular) {
		affine_curve = false;
	}


	//Create the initializer;
	trajectory_t init = {

		//An empty linked element;
		.link = EMPTY_LINKED_ELEMENT(),

		//The trajectory's curve;
		.curve = *curve,

		//Increments data;
		.increments = increments,

		.affine_curve = affine_curve,

	};

	//Initialise the trajectory;
	memcpy(trajectory, &init, sizeof(init));

	//If jerk monitoring is enabled :
	if (controller->jerk_monitoring_enabled) {

		//Initialise the jerk data;
		jerk_init(&trajectory->jerk_data, &geometric_base, increments.initial, affine_curve);

	} else {

		//Initialise the jerk struct to 0; pointers will be null and delete will be safely callable;
		memset(&trajectory->jerk_data, 0, sizeof(jerk_data_t));
	}


	//Adding a trajectory requires the alteration of the trajectory controller, which is not safe.
	//We will let the actuation clean up, and add the trajectory.
	actuation_add_trajectory(actuation, trajectory);

}


/*
 * trajectory_delete : deletes previously allocated data and frees the trajectory struct;
 */

void trajectory_delete(trajectory_t *const trajectory) {

	//Call the implementation deletion;
	(*(trajectory->deletion_function))(trajectory);

	//Delete jerk data;
	jerk_delete(&trajectory->jerk_data);

	//Free the trajectory structure;
	kernel_free(trajectory);

}


//----------------------------------------------------- Increments -----------------------------------------------------

/*
 * determine_increments : this function is the only public method of the class.
 *
 * It computes the increment for initial and final positions, for the given trajectory
 *
 * It handles the micro trajectory case.
 */

bool curve_determine_increments(const geometric_base_t *const geometric_base,
								trajectory_increments_t *const increments) {

	//Cache the curve;
	const curve_t *const curve = geometric_base->curve;

	//Cache indices
	float initial_index = curve->initial_index, final_index = curve->final_index;

	//Cache increments;
	float initial_incr = 0, final_incr = 0;



	//A flag determining if the trajectory is invalid;
	bool invalid;

	//If the index is ascending :
	if (initial_index < final_index) {

		//Determine the initial increment;
		initial_incr = curve_extract_increment(geometric_base, initial_index, 1);
		final_incr = curve_extract_increment(geometric_base, final_index, -1);

		//If the two basic trajectorys overlap each other, the trajectory is a micro trajectory (invalid)
		invalid = initial_index + initial_incr > final_index + final_incr;


	} else {

		//If the index is descending :
		initial_incr = curve_extract_increment(geometric_base, initial_index, -1);
		final_incr = curve_extract_increment(geometric_base, final_index, 1);

		//If the two basic trajectorys overlap each other, the trajectory is a micro trajectory (invalid);
		invalid = initial_index + initial_incr < final_index + final_incr;

	}

	//If the trajectory has been marked invalid :
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
 * 		- dimension : the trajectory's dimension;
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
 *  Literally, this function is called during the scheduling of a trajectory, when the increment variable
 *      must be determined.
 *
 */

float curve_extract_increment(const geometric_base_t *const geometric_base, const float point, float increment) {

	//Cache the distance target;
	const uint16_t distance_target = geometric_base->dbounds.target;

	//Cache the dimension;
	const uint8_t dimension = geometric_base->dimension;

	//Arrays initialisation
	float initial_positions[dimension], positions[dimension];

	//Get initial positions
	curve_evaluate_convert(geometric_base, point, initial_positions);

	//Cache float version of distance_target
	float ftarget = (float) distance_target;


	//Loop to execute to correct the increment;
	do {

		//Get the candidate actuation position;
		curve_evaluate_convert(geometric_base, point + increment, positions);

		//Get the distance between the initial distance and the candidate position.
		float distance = get_max_dist(dimension, initial_positions, positions);

		//Catch the case where d becomes 0.
		if (distance < 1) distance = 1;

		//If the distance target is reached :
		if ((uint16_t) distance == distance_target) {

			//return the increment that gave the correct increment;
			return increment;

		}

		//Update the increment
		increment *= ftarget / distance;


		//While the increment has not been reached;
	} while (1);

}


/*
 * get_max_dist : this function gets the maximal distance between the two provided positions, p0 and p1.
 *
 *  For each axis, it determines the real distance (float), and determines the absolute integer distance.
 */

float get_max_dist(uint8_t dimension, const float *p0, const float *p1) {

	//Initialise_hardware the maimum distance
	float max_dist = 0;

	//For each axis
	while (dimension--) {

		//Get the algebraic distance
		float distance = *(p1++) - *(p0++);

		//obtain the absolute distance
		if (distance < 0) {
			distance = -distance;
		}

		//Update max_dist
		if (distance > max_dist) {
			max_dist = distance;
		}

	}

	//Return the maximum distance.
	return max_dist;

}


