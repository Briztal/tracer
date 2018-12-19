/*
  planner.h Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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

#ifndef TRACER_PLANNER_H
#define TRACER_PLANNER_H


#include <stddef.h>

#include <dmz/list.h>


//------------------------------------------------------- States -------------------------------------------------------

/**
 * The planner state enum defines all possible modes of the planner.
 *
 * Authorised transitions are :
 * 	- STOP -> HOOK;
 * 	- HOOK -> TRAJECTORY;
 * 	- TRAJECTORY -> HOOK;
 * 	- TRAJECTORY -> STOP;
 */

enum planner_state {
	
	//The planner is stopped. Last time it computed a point, it finished processing all trajectories.
		PLANNER_STOP_MODE,
	
	//The planner is in hook mode; It will hook the next transition point;
		PLANNER_HOOK_MODE,
	
	//The planner is in trajectory mode, it will process the current trajectory;
		PLANNER_TRAJECTORY_MODE,
	
};


//-------------------------------------------------- Distance function -------------------------------------------------

/**
 * A distance function computes a distance from two points of the same size;
 */

typedef float (*distance_f)(
	
	//The array containing the coordinates of the first point;
	float p0[],
	
	//The array containing the coordinates of the second point;
	float p1[],
	
	//The size of both arrays;
	size_t arrays_size
	
);


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
	
	//The dimension of the trajectory;
	const size_t t_dim;
	
	//The start index, minimal value of the index;
	const float t_imin;
	
	//The stop index, maximal value of the index;
	const float t_imax;
	
	//An index increment that can cause significant increments on point coordinates;
	const float t_dincr;
	
	//The trajectory function;
	void (*const r_function)(float index, float *dst_array, size_t dst_size);
	
};


//Evaluate the trajectory at one point;
void trajectory_evaluate(struct trajectory *traj, float index, float *dst_array, size_t dst_size);


//----------------------------------------------------- Planner element -----------------------------------------------------

/**
 * In order to facilitate the build and usage of complex trajectory objects, the planner doesn't receive trajectories
 * 	(that can't be composed this way) but planner objects, that can be composed freely.
 */

struct planner_elmt {
	
	//The trajectory that the planner should follow;
	struct trajectory *pe_traj;
	
};


//----------------------------------------------------- Transition -----------------------------------------------------


/**
 * A singularity represents a sudden change of direction, from dir0 to dir1, defined by their coordinates, a at
 * 	certain position;
 *
 * 	The transition struct only provides base data : point and directions coordinates.
 *
 * 	A regulation algorithm will use this data as a base and compute its proprietary data from it.
 */

struct singularity {
	
	//A list head, for linkage
	struct list_head s_head;
	
	//A singularity can contain no information. Ex : a transition from stopped to hook.
	// In this case, this flag will be cleared.
	bool consistent;
	
	//Singularities are linked altogether. External software must know where is the end of the list;
	bool stop_transition;
	
	//The position of the direction change point;
	float s_pos[];
	
	//Movement direction before the direction change;
	float s_dir0[];
	
	//Movement direction, after the direction change;
	float s_dir1[];
	
};


/**
 * A transition represents a change of state in the planner.
 */

struct transition {
	
	//A list head, for storage in the planner;
	struct list_head t_head;
	
	//The state of the planner after the transition;
	enum planner_state t_state;
	
	//The planner element, in case of trajectory mode;
	struct planner_elmt *t_elmt;
	
	//The related singularity;
	struct singularity *t_sing;
	
};


/**
 * 	As proprietary data shall be computed for each transition, the transition struct itself is incomplete.
 *
 * 	The regulation algorithm will be in charge of defining its proprietary struct, composed by a transition struct,
 * 	to be compliant with the planner.
 *
 * 	The regulation algorithm will be in charge of creating, updating, and deleting those structs, via the transition
 * 	factory structure defined below;
 */

struct singularity_factory {
	
	//Create a transition.
	struct irregular_point *(*create_singularity)(struct singularity_factory *factory, size_t dimension);
	
	//Update proprietary data; The factory, the transition to update,
	// and the ref of the list's first transition are provided
	void (*update_singularity)(
		struct singularity_factory *factory,
		struct singularity *sing,
		const struct singularity *first_sing
	);
	
	//Delete a transition;
	void (*delete_singularity)(struct singularity_factory *factory, struct irregular_point *trans);
	
};


//------------------------------------------------------- Planner ------------------------------------------------------

/**
 * The planner has access to a set of constants that determine its behaviour.
 */

struct planner_constants {
	
	//The size of the planner;
	const size_t p_size;
	
	//The size of the continuous trajectory history; Must not be zero;
	const size_t traj_history_size;
	
	//The minimal distance bound;
	const float p_dmin;
	
	//The target distance;
	const float p_dtarget;
	
	//The maximal distance bound;
	const float p_dmax;
	
};


/**
 * During the search in a trajectory, two variables are used and updated;
 */

struct search_vars {
	
	//The estimated index to start the search at;
	float p_index;
	
	//The increment to apply if estimation is incorrect;
	float p_increment;
	
	//The distance function;
	const distance_f p_distf;
	
};


/**
 * The planner struct keeps track of all data required for the planning process. You can find a detailed explanation
 * 	about each part on the code's documentation.
 */

struct planner {
	
	//The current state of the planner;
	enum planner_state p_state;
	
	//The set of planner constants;
	struct planner_constants csts;
	
	//The next transition;
	struct transition *p_trans;
	
	//The last transition. Redundant, for optimisation purposes;
	//TODO
	
	
	//The remaining number of spaces in the history;
	size_t history_spaces;
	
	//The next singularity to encounter;
	struct singularity *p_sing;
	
	//The search variables;
	struct search_vars *p_svars;
	
	//The singularity struct factory;
	struct singularity_factory *p_sfactory;
	
};


//Add a trajectory to the planner's list;
void planner_add_element(struct planner *pl, struct planner_elmt *elmt);

//Get the list of trajectories to recycle; Null is returned if no element is available;
struct trajectory *planner_get_old_elements(struct planner *pl);


/**
 * planner_get_point : Execute an iteration in the discretization, from a current position, and a cost target;
 *
 * Resulting coordinates will be stored in dst_array;
 *
 * Depending on the current mode of the planner, either the current trajectory will be processed, or the hook
 * algorithm will be used to determine a point.
 *
 * @param pl : the planner;
 * @param pos_array : the array containing the current position; Not asserted to belong to any trajectory;
 * @param cost_target : the cost function's target;
 * @param dst_array : the array containing the current position;
 * @param size : the size of both arrays;
 * @return true if a point was successfully computed, false if all points of all trajectories have been computed;
 */

bool planner_get_point(struct planner *pl, const float pos_array[], float dst_array[], size_t size);






#endif //TRACER_PLANNER_H
