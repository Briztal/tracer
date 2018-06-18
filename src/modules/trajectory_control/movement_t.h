//
// Created by root on 6/18/18.
//

#ifndef TRACER_MOVEMENT_T_H
#define TRACER_MOVEMENT_T_H


#include <data_structures/containers/llist.h>


/*
 * movement_data : this structure contains all data related to one movement :
 *  - the index variables : beginning, ending, increment;
 *  - the initialisation and finailisation function, called (resp) at the beginning and ending of the movement;
 *  - the trajectory function, used continuously during the movement to get positions;
 *  - The regulation_speed variables : the regulation_speed and the regulation_speed group,
 *  - The tools signature, indicating tools enabled during this movement.
 *
 */


/*
 * The trajectory;
 */
typedef struct {

	//The trajectory function : it takes a real value (the index) and outputs a position;
	void (*const trajectory_function)(struct movement_t *mvmt, float index, float *positions);

	//The beginning value of the trajectory index;
	const float beginning;

	//The ending value of the trajectory index;
	const float ending;

	//The increment at the beginning point;
	float beginning_increment;

	//The increment at the ending point;
	float ending_increment;

} movement_equation_t;



typedef struct movement_t {

	//Movements are stored in linked lists;
	linked_element_t link;

	//The dimension of the movement;
	const uint8_t dimension;

	//Movement equation structure;
	const movement_equation_t equation;

	//uint8_t speed_group;

	//TODO PROPER INITIALISATION;

	/*
	 * Jerk :
	 *
	 *  The following lines contain all data required to perform the jerk regulation
	 */

	/*
	//A boolean, determining whether the ending point is a jerk point
	bool jerk_point = false;

	//The position of the jerk point
	int32_t jerk_position[NB_STEPPERS]{0};

	//The jerk ratios, at the end of the movement.
	float ending_jerk_ratios[NB_STEPPERS];
	*/

	/*
	 * Tools data : the following signature determines which of tools will prepare_movement during the movement.
	 */

	//sig_t tools_signatures;

} movement_t;


//Initialise a movement with its primary data;
void movement_init(movement_t *movement, uint8_t dimension, void (*trajectory_function)(movement_t *, float ,float *),
				   float beginning, float ending);

//Delete a movement's dynamic data;
void movement_delete(movement_t *)



#define MOVEMENT_EQUATION(trajectory, begin_index, ending_index, begin_incr, ending_incr) {\
	.trajectory_function = (trajectory),\
	.beginning = (begin_index),\
	.ending = (ending_index),\
	.beginning_increment = (begin_incr),\
	.ending_increment = (ending_incr),\
}


//

#endif //TRACER_MOVEMENT_T_H
