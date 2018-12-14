//
// Created by root on 12/13/18.
//

#ifndef TRACER_PLANNER_H
#define TRACER_PLANNER_H


#include <stddef.h>

#include <dmz/list.h>

struct trajectory;
struct planner;


//----------------------------------------------------- Trajectory -----------------------------------------------------

/**
 * An application t is a trajectory if it is defined as :
 *
 * 	t : [a, b] -> R^n, a and b two real numbers, n a positive integer;
 *
 * 	naming :
 * 	- n : the dimension of the trajectory;
 * 	- x in [a, b] : an index of the trajectory;
 * 	- t(x), x in [a, b] : a point of the trajectory;
 * 	- a : the minimal index, the initial index;
 * 	- t(a) : the initial point;
 * 	- b : the maximal index, the final index;
 * 	- t(b) : the final point;
 */


struct trajectory {
	
	//Trajectories are linked together;
	struct list_head head;
	
	//The dimension of the trajectory;
	const size_t dimension;
	
	//The start index, minimal value of the index;
	const float t_imin;
	
	//The stop index, maximal value of the index;
	const float t_imax;
	
	//An index increment that can cause significant increments on point coordinates;
	const float t_default_incr;
	
	//The trajectory function;
	void (*const r_function)(float index, float *dst_array, size_t dst_size);
	
};


//Evaluate the trajectory at one point;
void trajectory_evaluate(struct trajectory *traj, float index, float *dst_array, size_t dst_size);


/**
 * trajectory_find : search the trajectory, from a particular point/increment, for a point whose cost is close enough
 * 	to the cost target;
 *
 * 	The cost is given from a cost function, that is fed with distances from a reference point
 *
 * @param traj : the trajectory to search in;
 * @param index : the initial index;
 * @param increment : the initial increment;
 * @param ref_array : the coordinates of the reference point;
 * @param point_array : the array where to compute points. Will contain coordinates of the point at the return index
 * @param size : the size of arrays;
 * @param cost_f : the cost function, giving a cost from a float array;
 * @param cost_target : the target of the cost function;
 * @param cost_threshold : the threshold to determine if a point is close enough to the cost target;
 * @param nb_iterations : the maximum number of iterations;
 * @return the index of the found point;
 */

float trajectory_find(
	struct trajectory *traj,
	float index,
	float *ref_array,
	float *point_array,
	size_t size,
	float (*cost_f)(float *, size_t),
	float cost_target,
	float cost_threshold,
	size_t nb_iterations
);


//------------------------------------------------------- Planner ------------------------------------------------------

/**
 * A planner is an algorithm whose purpose is to discretize on the fly a sequence of trajectory in a sequence of points.
 *
 * 	It stores references to trajectories and discretizes them integrally in their order of adding.
 *
 * 	The discretization process is determined by a cost function, that determines which point must be selected, given
 * 	the current position (not necessarily belonging to the current trajectory) and a cost target;
 *
 * 	When the planner detects that the final point of the trajectory is close enough of the current point, it provided
 * 	it. For this reason, the final point may be more far from its predecessor than others.
 *
 * 	It can be done for both points, indeed, the final point of the trajectory can be the initial point of the next one.
 * 	The cost function and its target will decide of which point to provide;
 */

struct planner {
	
	//The dimension of the planner.
	const size_t p_size;

	//The current trajectory;
	struct trajectory *p_traj;
	
	//The current index in the current trajectory;
	float p_index;

	//The current increment in the current trajectory;
	float p_incr;
	
};


//Add a trajectory to the planner's list;
void planner_add_trajectory(struct planner *pl, struct trajectory *traj);


/**
 * planner_get_point : Execute an iteration in the discretization, from a current position, and a cost target;
 *
 * Resulting coordinates will be stored in dst_array;
 *
 * The current trajectory will be traversed from the last provided point.
 *
 *
 * @param pl : the planner;
 * @param pos_array : the array containing the current position; Not asserted to belong to any trajectory;
 * @param cost_target : the cost function's target;
 * @param dst_array : the array containing the current position;
 * @param size : the size of both arrays;
 * @return true if a point was successfully computed, false if all points of all trajectories have been computed;
 */

bool planner_get_point(struct planner *pl, const float pos_array[], float cost_target, float dst_array[], size_t size);





#endif //TRACER_PLANNER_H
