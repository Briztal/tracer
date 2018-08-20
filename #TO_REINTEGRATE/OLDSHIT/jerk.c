
#include <kernel/kernel.h>
#include <string.h>
#include "jerk.h"
#include "trajectory.h"


//--------------------------------------------------- Private headers --------------------------------------------------

//Determine distances using only initial and final position;
void jerk_determine_bounds_distances(const geometric_base_t *geometric_base, uint32_t *distances);

//Determine distances by traversing the curve; Slow;
void jerk_determine_traversal_distances(const geometric_base_t *geometric_base, float initial_increment,
										uint32_t *distances);


//----------------------------------------------------- Distances -----------------------------------------------------

/*
 * trajectory_jerk_init : Initialises the three fields of the provided jerk struct;
 */

void trajectory_jerk_init(trajectory_jerk_t *jerk_data, const geometric_base_t *geometric_base, float initial_increment,
						  bool affine_curve) {

	//Deactivate the jerk point;
	jerk_data->jerk_flags = 0;


	//Cache the length;
	size_t len = ((size_t) geometric_base->dimension) * sizeof(float);


	//Allocate trajectory distances;
	uint32_t *trajectory_distances = kernel_malloc(len);

	//If actuator trajectories are all affine :
	if (affine_curve) {

		//Compute distance between begin and end point;
		jerk_determine_bounds_distances(geometric_base, trajectory_distances);

	} else {

		//If not, traverse the trajectory and compute the jerk;
		jerk_determine_traversal_distances(geometric_base, initial_increment, trajectory_distances);

	}

	//Save the trajectory distances;
	jerk_data->trajectory_distances = trajectory_distances;


	//Allocate jerk offsets;
	uint32_t *jerk_offsets = kernel_malloc(len);

	//Initialise jerk offsets to zero;
	memcpy(jerk_offsets, 0, len);

	//Save jerk offsets;
	jerk_data->jerk_offsets = jerk_offsets;

}


void trajectory_jerk_delete(trajectory_jerk_t *jerk_data) {

	//Free the trajectory array;
	kernel_free(jerk_data->trajectory_distances);

	//Free the offsets array;
	kernel_free(jerk_data->jerk_offsets);

}



/*
 * jerk_determine_bounds_distance : compute the trajectory distance only using initial and final positions;
 */

void jerk_determine_bounds_distances(const geometric_base_t *const geometric_base, uint32_t *distances) {

	//Cache the dimension;
	const uint8_t dimension = geometric_base->dimension;

	//Cache the curve;
	const curve_t *const curve = geometric_base->curve;

	//Declare a temporary array for control position;
	float tmp[dimension];

	//First, declare arrays for the initial and final positions;
	float initial_position[dimension], final_position[dimension];

	//Determine the initial position;
	curve_evaluate_convert_tmp_provided(geometric_base, tmp, curve->initial_index, initial_position);

	//Determine the final position;
	curve_evaluate_convert_tmp_provided(geometric_base, tmp, curve->final_index, initial_position);

	//For each actuator :
	for (uint8_t axis = dimension; axis--;) {

		//Determine the float distance;
		float distance = final_position[axis] - initial_position[axis];

		//Determine the absolute value of the distance;
		if (distance < 0) distance = -distance;

		//Save the distance in the distances array;
		distances[axis] = (uint32_t) distance;

	}

}




//Determine distances by traversing the curve; Slow;
void jerk_determine_traversal_distances(const geometric_base_t *const geometric_base, const float initial_increment,
									   uint32_t *const distances) {

	//Not supported for instance;
	kernel_error("trajectory.c : curve_determine_distances : complex movements are not supported for instance. "
					 "Curve distance comput must be implemented before;");

}


//---------------------------------------------------- Jerk monitor ----------------------------------------------------
