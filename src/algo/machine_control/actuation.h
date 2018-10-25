//
// Created by root on 10/1/18.
//

#ifndef TRACER_ACTUATION_H
#define TRACER_ACTUATION_H

#include "stdint.h"

/*
 * Actuators have physical limitations, that we must absolutely respect, otherwise, actuators or the machine could
 * 	be damaged;
 *
 * 	An actuator physics model is an interface, containing two functions :
 * 	- one to compute the largest acceptable duration interval for a movement of a given distance, given
 * 		the current state of the machine;
 * 	- one to compute a distance that matches physical limitation, for a movement of a given time, given the current
 * 		state of the machine state and the target distance;
 *  - A supplementary flag can be used to signal that the distance on this axis can be adjusted to comply with
 *  	physical limitations;
 */


typedef void (*actuation_duration_cmp)(
	
	//The actuator's movement distance;
	int32_t distance,
	
	//The machine's current state, constant;
	const struct mstate *current_state,
	
	//The location where to store the duration window, mutable;
	struct time_interval *const duration_window);


typedef void (*actuation_min_distance_cmp)(
	
	//The movement's duration, constant;
	const float movement_duration,
	
	//The pointer referencing the distance target. Must be updated with the calculated distance, mutable;
	int32_t *const distance_target);



struct actuator_model {
	
	/*
	 * The function to compute the time interval; takes following arguments;
	 * 	This function pointer can be null; If so, the duration interval will be set to its maximum;
	 */
	const actuation_duration_cmp compute_duration_interval;
	
	
	/*
	 * The function to compute the minimal distance to fit duration requirements;
	 * 	This function pointer can be null; If so, no distance correction will happen;
	 */
	const actuation_min_distance_cmp compute_minimal_distance;
	
};

#endif //TRACER_ACTUATION_H
