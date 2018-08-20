/*
  trajectory.h Part of TRACER

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


#ifndef TRACER_TRAJECTORY_H
#define TRACER_TRAJECTORY_H

#include "stdint.h"

#include "modules/machine_control/actuation.h"

#include "modules/machine_control/geometry.h"
#include "jerk.h"

#include <data_structures/containers/non_concurrent/llist.h>



//The curve function type; Receives a curve data and an index and updates the positions array;
typedef void (*curve_function_t)(struct curve_t *curve, float index, float *positions);

/*
 * The parametric equation struct;
 */
typedef struct {

	//The curve's dimension;
	const uint8_t dimension;

	//The trajectory function : it takes a real value (the index) and outputs a position;
	const curve_function_t function;

	//The initial value of the trajectory index;
	const float initial_index;

	//The final value of the trajectory index;
	const float final_index;

} curve_t;


//Get a particular point of the curve;
static void curve_evaluate(const curve_t *curve, const float index, float *const dest) {

	//Get the function, and call it with all args;
	(*(curve->function))(curve, index, dest);

}


//--------------------------------------------------- Geometric base ---------------------------------------------------

/*
 * This struct eases arguments passing during geometric computations;
 */

typedef struct {

	//The dimension (duplicate of the curve dimension);
	uint8_t dimension;

	//The curve pointer;
	const curve_t *const curve;

	//The geometry pointer;
	const geometry_t *const geometry;

	//The distance bounds;
	const dbounds_t dbounds;

} geometric_base_t;


//Evaluate the curve in one point and translate the result in control coordinates;
void curve_evaluate_convert(const geometric_base_t *geometric_base, float index, float *dest);


//Evaluate the curve in one point and translate the result in control coordinates; Uses a provided temporary array;
void curve_evaluate_convert_tmp_provided(const geometric_base_t *geometric_base, float *tmp, float index, float *dest);


//----------------------------------------------------- Trajectory -----------------------------------------------------

/*
 * Increments struct : contains initial and final increments;
 */
typedef struct {

	//The increment at the initial point;
	float initial;

	//The increment at the final point;
	float final;

} trajectory_increments_t;



/*
 * A trajectory is composed of :
 * 	- a linked element on is base, to allow storage in linked lists;
 * 	- a dimension;
 * 	- a curve, determining the path;
 * 	- base increments for initial an final trajectories;
 * 	- storage data for jerk management;
 */

typedef struct trajectory_t {

	//Trajectories are stored in linked lists;
	linked_element_t link;

	//Trajectory curve;
	const curve_t curve;

	//A flag to determine if the distance comput can be optimised;
	bool affine_curve;

	//Increments;
	trajectory_increments_t increments;

	//Jerk data;
	trajectory_jerk_t jerk_data;


	//Deletion function;
	void (*deletion_function)(struct trajectory_t *);

} trajectory_t;


//Initialise a trajectory with its primary data, and send it to the actuation layer;
void trajectory_init(trajectory_t *trajectory, const curve_t *curve, bool affine_curve, actuation_t *actuation);

//Delete a trajectory's dynamic data;
void trajectory_delete(trajectory_t *trajectory);


#endif //TRACER_TRAJECTORY_H
