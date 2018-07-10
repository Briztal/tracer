//
// Created by root on 7/10/18.
//

#ifndef TRACER_ACTUATION_PHYSICS_H
#define TRACER_ACTUATION_PHYSICS_H

#include <stdbool.h>

#include <stdint.h>

#include "machine.h"

/*
 * Actuators have physical limitations, that we must absolutely respect, otherwise, actuators or the machine could
 * 	be damaged;
 *
 * 	TODO DOC ON TIME WINDOW CMPUTATION, IN CASE OF IMPOSSIBLE TIME (TURNAROUND), PROVIDE A TIME THAT WILL NOT CHANGE ABSOLUTE DISTANCE AFTER DISTANCE RECOMPUTATION;
 */



//------------------------------------------------- Arguments structure ------------------------------------------------

/*
 * The duration interval computation function requires the following arguments :
 */

typedef struct {

	//The actuator physics instance;
	void *instance;

	//The machine's current state, constant;
	const machine_state_const_t *const machine_state;

	//The movement's distance on the actuator;
	const uint16_t distance;

	//The time interval where to store the duration window, mutable;
	time_interval_t *const duration_window;

} actuator_duration_args_t;


/*
 * The distance computation function requires the following arguments :
 */

typedef struct {

	//The actuator physics instance, mutable;
	void *const instance;

	//The movement's duration, constant;
	float movement_duration;

	//The pointer referencing the distance target. Must be updated with the calculated distance, mutable;
	int16_t *distance_target;

} actuator_distance_args_t;


//---------------------------------------------- Actuation physics struct ----------------------------------------------

/*
 * 	An actuator physics controller is an interface, containing an instance and two functions :
 * 	- one to compute the largest acceptable duration interval for a movement of a given distance, given
 * 		the current state of the machine;
 * 	- one to compute a distance that matches physical limitation, for a movement of a given time, given the current
 * 		state of the machine state and the target distance;
 */

typedef struct {

	//The instance to pass when the duration interval is computed;
	void *const instance;

	//The function to compute the time interval;
	void (*const compute_duration_interval)(actuator_duration_args_t *args);

	//The function to compute the minimal distance to fit duration requirements;
	void (*const compute_minimal_distance)(actuator_distance_args_t *args);

	//The adjustment flag. If set, distance can be adjusted to comply with the physical constraint;
	const bool distance_adjustable;

} actuator_physics_t;


#endif //TRACER_ACTUATION_PHYSICS_H
