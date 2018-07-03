//
// Created by root on 6/25/18.
//

#ifndef TRACER_KINEMATICS_H
#define TRACER_KINEMATICS_H

#include <stdint.h>
#include "jerk.h"


/*
 * Maximal acceleration and deceleration can be correlated or not. Anyway, they are computed at the same time.
 *
 * 	This struct avoids passing multiple parameters;
 */

typedef struct {

	//The actuator identifier;
	uint8_t actuator;

	//The actuator speed;
	float actuator_speed;

	//The actuator maximal acceleration;
	float max_acceleration;

	//The actuator maximal deceleration;
	float max_deceleration;

} kinematics_acceleration_data_t;


/*
 * Stop distance computation can involve the current speed, or the current speed variation, depending on the method
 * 	used;
 */
typedef struct {

	//The actuator identifier;
	uint8_t actuator;

	//The actuator's speed;
	float previous_actuator_speed;

	//The actuator's previous;
	float actuator_speed;

	//The current stop distance
	float *stop_distance;

} kinematics_stop_distance_data_t;

/*
 * The kinematics struct contains functions that are in charge of providing kinematic parameters for a given speed;
 */

typedef struct kinematics_t {

	//--------------------------------------- Configuration ---------------------------------------

	//Is the acceleration regulation supported ?
	const bool acceleration_regulation_supported;

	//Is the deceleration planing supported ?
	const bool deceleration_planning_supported;

	//Is the jerk regulation supported ?
	const bool jerk_regulation_supported;


	//--------------------------------------- Computation ---------------------------------------

	//Set this flag to trigger a maximal deceleration on next movement;
	bool deceleration_required;

	//The previous movement's duration;
	float previous_movement_duration;

	//Axis current speeds;
	float *const actuators_speeds;

	//Distances required to decelerate;
	float *const stop_distances;

	//Distances to jerk point;
	uint32_t *const jerk_distances;

	//Jerk offsets (deceleration distance to limit speed) at the end point;
	uint32_t *const jerk_offsets;


	//--------------------------------------- Functions ---------------------------------------

	//---------------- Acceleration - Deceleration ----------------

	//The function to compute maximal acceleration and deceleration;
	float (*const compute_acceleration_bounds)(struct kinematics_t *kinematics, kinematics_acceleration_data_t *data);


	//---------------- Stop distance ----------------

	//The function to compute the stop distance on the actuator at the provided speed, without integration;
	void (*const compute_stop_distance)(struct kinematics_t *kinematics, kinematics_stop_distance_data_t *data);

	//Sometimes integration can be more efficient than direct evaluation to actualise the stop distance;
	//This signature determines whether or not the integration method is more efficient for each axis;
	const sig_t stop_distance_integration_enabled;

	//If the integration method is more efficient than a direct calculation, this will update the stop distance;
	void (*const update_integrated_stop_distance)(struct kinematics_t *kinematics,
												   kinematics_stop_distance_data_t *data);


	//---------------- Maximal jerk speed ----------------

	//The function to compute the maximal accelerating jerk for one actuator; Its speed must be provided;
	//It solves the equation "|coefficient| * V = jerk(V, coefficient)";
	float (*const compute_max_jerk_speed)(uint8_t axis, float equation_coefficient);


} kinematics_t;


//Initialise kinematics, before computing the first movement;
void kinematics_init();


//Update the jerk struct after a trajectory has been completed;
void kinematics_update_jerk_data(kinematics_t *kinematics, const linked_list_t *trajectories);


//Compute a movement's duration according to the current context;
void kinematics_compute_movement_data(kinematics_t *kinematics, const mbuilder_t *mbuilder, movement_t *movement);

//Update jerk distances after a movement has been computed;
void kinematics_update_jerk_distances(kinematics_t *kinematics, uint8_t dimension, const movement_t *movement);

//Update actuator speeds according to the new movement time;
void kinematics_update_axis_speeds(kinematics_t *kinematics, const movement_t *movement);


#endif //TRACER_KINEMATICS_H
