//
// Created by root on 6/24/18.
//

#ifndef TRACER_MBUILDER_T_H
#define TRACER_MBUILDER_T_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>


/*
 * During a movement's division, we must keep track of the current index, and data associated;
 */
typedef struct {

	//The current index, corresponding to the current position;
	float index;

	//The current increment; Can be modified if the next elementary movement is too close/far;
	float increment;

	//A flag determining if the index increases during the movement;
	bool increasing_index;

	//The index max/min value;
	float limit_index;

	//The index candidate, corresponding to the end of the next movement;
	float index_candidate;

	//A flag, determining if the limit index is actually reached;
	bool limit_index_reached;

} index_monitor_t;


/*
 * There are some steps requried to build a movement. These comprise :
 * 	- The selection of the next index;
 * 	- The determination of current position;
 * 	- The determination of movement's distances;
 * 	- The validation of distance bounds;
 * 	- The index update in case of valid movement;
 * 	- The determination of movement time, according to kinetics parameters;
 */

typedef struct {

	//The trajectory index data;
	index_monitor_t index_monitor;


	//Control position to go to;
	float *const control_destination;


	/*
	 * Arrays for actuation position;
	 */

	//Actuation arrays
	float *const actuation_t0, *const actuation_t1;

	//Does t0 array currently represent current actuation position ?
	bool current_is_t0;


	/*
	 * Distances
	 */

	//The actuation distances;
	float *const actuation_distances;

	//The maximal distance;
	float maximal_distance;

} mbuilder_t;


//Create control / actuation position arrays, initialise the struct;
void mbuilder_init(mbuilder_t *builder, size_t dimension);

//Free mbuilder arrays;
void mbuilder_delete(mbuilder_t *mbuilder);

//Switch current/previous actuation positions arrays;
void mbuilder_switch_actuation_arrays(mbuilder_t *mbuilder);


/*
 * mbuilder_get_actuation_positions : Get current position arrays;
 */

inline void mbuilder_get_actuation_positions(mbuilder_t *mbuilder, const float **current, float **next) {

	//If t0 represents the current position :
	if (mbuilder->current_is_t0) {

		//t0 is assigned to the current;
		*current = mbuilder->actuation_t0;

		//t1 is assigned to the next;
		*next = mbuilder->actuation_t1;

	} else {
		//If t0 represents the previous :

		//t1 is assigned to the current;
		*current = mbuilder->actuation_t1;

		//t0 is assigned to the next;
		*next = mbuilder->actuation_t0;
	}

}


/*
 * mbuilder_get_actuation_destination : Get current position arrays;
 */

inline void mbuilder_get_actuation_destination(mbuilder_t *mbuilder, float **next) {

	//If the current is t0, assign to t1. If not, assign to t0,
	*next = (mbuilder->current_is_t0) ? mbuilder->actuation_t1 : mbuilder->actuation_t0;

}


#endif //TRACER_MBUILDER_T_H
