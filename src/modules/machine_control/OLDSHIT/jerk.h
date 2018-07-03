//
// Created by root on 6/27/18.
//

#ifndef TRACER_JERK_H
#define TRACER_JERK_H

#include <stdint.h>

#include "movement.h"
#include "trajectory.h"


//--------------------------------------------------- Trajectory Jerk --------------------------------------------------


/*
 * The trajectory jerk data struct;
 *
 * To regulate the jerk between trajectories, we must know for each actuator :
 * 	- whether or not the actuator comprises a jerk point at the trajectory's end;
 * 	- the trajectory's length;
 * 	- the deceleration distance at the jerk point's maximal acceptable speed, called jerk offset;
 */

typedef struct {

	//A signature, determining which actuator must monitor jerk on the trajectory's end point; Can be modified;
	sig_t jerk_flags;

	//Distance of the trajectory for each actuator; Set once;
	uint32_t *trajectory_distances;

	//Jerk offsets (deceleration distance to limit speed) at the end point; Set once;
	uint32_t *jerk_offsets;

} trajectory_jerk_t;


//Initialise the jerk structure in the trajectory;
void trajectory_jerk_init(trajectory_jerk_t *jerk_data, const geometric_base_t *geometric_base, float initial_increment,
						  bool affine_curve);

//Release jerk data;
void trajectory_jerk_delete(trajectory_jerk_t *jerk_data);




#endif //TRACER_JERK_H
