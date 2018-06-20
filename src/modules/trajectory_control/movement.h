/*
  movement.h Part of TRACER

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


#ifndef TRACER_MOVEMENT_H
#define TRACER_MOVEMENT_H

#include "stdint.h"
#include "trajectory.h"
#include "curve.h"

#include <data_structures/containers/llist.h>


/*
 * movement_data : this structure contains all data related to one movement :
 *  - the index variables : initial, final, increment;
 *  - the initialisation and finailisation function, called (resp) at the initial and final of the movement;
 *  - the trajectory function, used continuously during the movement to get positions;
 *  - The regulation_speed variables : the regulation_speed and the regulation_speed group,
 *  - The tools signature, indicating tools enabled during this movement.
 *
 */

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
 * The jerk data struct;
 */
typedef struct {

	//A boolean, determining whether the final point is a jerk point
	bool jerk_point;

	//The position of the jerk point
	int32_t *jerk_position;

	//The jerk ratios, at the end of the movement.
	float *ending_jerk_ratios;

} jerk_data_t;


/*
 * A movement is composed of :
 * 	- a linked element on is base, to allow storage in linked lists;
 * 	- a dimension;
 * 	- a curve, determining the movement's path;
 * 	- base increments for initial an final elementary movements;
 * 	- storage data for jerk management;
 */

typedef struct movement_t {

	//Movements are stored in linked lists;
	linked_element_t link;

	//The dimension of the movement;
	const uint8_t dimension;

	//Movement curve;
	const curve_t curve;

	//Increments;
	trajectory_increments_t increments;

	//Jerk data;
	jerk_data_t jerk_data;

	//Deletion function;
	void (*deletion_function)(struct movement_t *);

} movement_t;


//Initialise a movement with its primary data, and add it to the trajectory;
void movement_init(movement_t *movement, uint8_t dimension, const curve_t *curve, tcontroller_t *trajectory);

//Delete a movement's dynamic data;
void movement_delete(movement_t *);


#endif //TRACER_MOVEMENT_H
