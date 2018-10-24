/*
  mcore.c Part of TRACER

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

#include "mcore.h"

#include <kernel/debug/log.h>


//------------------------------------------------- Computation stages -------------------------------------------------


/*
 * The computation of a new movement is divided in several consecutive stages :
 *
 * 	0 - distances computation :
 * 		The machine controller computes the new movement's distances, given the current state;
 * 		During this computation, it may pre-calculate some data of the new state.
 *
 * 		This function is a one liner written in the computation procedure;
 */


/*
 * 	1 - duration intervals computation :
 * 		Each actuator's physical model computes the duration interval for each axis;
 */

static void compute_durations(uint8_t dimension, struct actuator_model **actuators,
							  const int16_t *distances, const struct mstate *current_state,
							  struct time_interval *dst);


/*
 *  2 - interval merge :
 * 		determines determine the best admissible interval, given the set of duration intervals;
 * 		If authorised, will activate the distance correction node for all required actuators;
 */

static bool merge_intervals(uint8_t dimension, struct time_interval *intervals, struct time_interval *dst_interval);


/*
 * 	3 - distance_correction :
 * 		for each axis, determines the closest distance from the original distance, that matches actuation constraints;
 * 		returns true if a distance is modified;
 */
static bool correct_distances(uint8_t dimension, struct actuator_model **actuators,
							  const struct time_interval *intervals, float duration, int16_t *distances);

/*
 *  4 - next position computation :
 * 		determines the next actuation position, given the current state, and the distances array.
 */

static void compute_next_position(uint8_t dimension, const int32_t *position, const int16_t *dists, int32_t *dests);


/*
 * 	5 -  geometric_model inversion :
 *		If a distance has been corrected, will invert the geometric_model, to get control positions from actuation
 *		positions.
 *
 *		One liner, written in computation function;
 */


/*
 *	6 - controller builder computation :
 *		the controller computes its part of the movement builder; It may realise any computation it requires for
 *		next stages;
 */
static void compute_builder(const struct mcontroller *controller,
							const struct mstate *current_state,
							const struct mstate *new_state,
							void *controller_data);


/*
 *	7 - duration interval reducing :
 *		If the duration interval for the current movement is not a singleton, the machine controller applies
 *		restrictions on this interval (based for ex on acceleration bounds, or jerk planning), until it becomes a
 *		singleton.
 *		If the duration interval is still not a singleton at the end of this stage, the maximal distance (lowest speed)
 *		is selected;
 */

static void reduce_interval(const struct mcontroller *controller,
							const struct mstate *current_state,
							const void *controller_data,
							struct time_interval *duration_window);

/*
 *	8 - controller state computation :
 *		the controller computes its part of the movement builder; It may realise any computation it requires to compute
 *		the next movement;
 */


static void compute_state(const struct mcontroller *controller,
						  const struct mstate *current_state,
						  const struct computation_data *cmp_data,
						  struct mstate *new_state);


//------------------------------------------------- Private utilities --------------------------------------------------

//Arbitrary duration selection in case of actuation model clash;
static void choose_duration(struct time_interval *final_interval, float previous_time);

//Mark time intervals invalid if so to trigger distance correction of related axis;
static void activate_corrections(uint8_t dimension, float time, struct time_interval *intervals);

//Switch states; The next becomes the current, the current is invalidated and becomes the next;
static void machine_states_accept(struct mstates *states);


//--------------------------------------------------- Init --------------------------------------------------


/**
 * machine_core_set_controller : registers the controller to the core.
 *
 * 	A dimension check is made, and if it succeeds, states and computation data structures are shared to the core;
 *
 * @param core : the machine core to initialise;
 *
 * @param controller : the machine controller to register to the core;
 */

void mcore_initialise(
	struct mcore *core,
	struct mstate *s0,
	struct mstate *s1,
	void *controller_computation_data
) {
	
	//Cache the core dimension;
	const uint8_t dimension = core->nb_axis;
	
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	//TODO ASSIGN CONTROLLER BUILDER AND STATE
	
	
	//Cache the first state dimension;
	uint8_t s0_dimension = s0->dimension;
	
	//If states are incompatible :
	if (s0_dimension != s1->dimension) {
		
		//Log;
		kernel_log_("mcore_initialise : state have different dimensions");
		
		//Fail;
		return;
		
	}
	
	
	//If controller dimensions are incompatible :
	if (dimension != s0_dimension) {
		
		//Log;
		kernel_log_("mcore_initialise : incompatible state dimensions");
		
		//Fail;
		return;
		
	}
	
	//Provide access to states;
	core->states.s0 = core->states.current_state = s0;
	core->states.s1 = core->states.next_state = s0;
	core->states.current_is_s0 = true;
	
	//Provide access to controller computation data;
	core->cmp_data.controller_data = controller_computation_data;
	
}


/*
 * Set the location where the computed movement should be stored;
 *
 * @param core : the movement core to update;
 * @param mvmt : the new movement struct; Can be null;
 */

void mcore_set_movement_dst(struct mcore *core, struct movement *mvmt) {
	
	//Update the computation destination; A null check will be made at execution time;
	core->movement = mvmt;
	
}




//------------------------------------------------- Computation stages -------------------------------------------------

/**
 * mcore_compute_movement : executes all stages in the computation of a movement, given a machine core;
 *
 * @param core : the machine core;
 */

bool mcore_compute_movement(struct mcore *const core) {
	
	/*
	 * Variables init and check stage;
	 */
	
	//Cache the movement container;
	struct movement *mvmt = core->movement;
	
	//If the movement container is null :
	if (!(mvmt)) {
		
		//Log;
		kernel_log_("mcore_compute_movement : movement output not provided");
		
		//Fail;
		return false;
		
	}
	
	//Cache the distances array;
	int16_t *const mv_distances = mvmt->actuation_distances;
	
	
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	//TODO REMOVE THE CONTROLLER
	
	//Cache the movement controller;
	struct mcontroller *ctrl = core->controller;
	
	//If the controller is nor initialised :
	if (!ctrl) {
		
		//Log;
		kernel_log_("mcore_compute_movement : controller not initialised");
		
		//Fail;
		return false;
		
	}
	
	//If the controller is not ready :
	if (!(ctrl->ready)) {
		
		//If one part of the controller is not initialised :
		if ((!(ctrl->geometry)) ||
			(!(ctrl->dist_computer)) ||
			ctrl->nb_uninitialised_actuators ||
			ctrl->nb_uninitialised_builder_cpts ||
			ctrl->nb_uninitialised_kinematic_cnsts ||
			ctrl->nb_uninitialised_state_cpts) {
			
			//Log;
			kernel_log_("mcore_compute_movement : controller not ready");
			
			//Fail;
			return false;
			
		} else {
			//If the controller is initialised but not marked ready yet :
			
			//Mark it ready and keep on;
			ctrl->ready = true;
			
		}
		
		
	}
	
	
	//Cache the dimension;
	const uint8_t dimension = core->nb_axis;
	
	//Cache the current state and the next state;
	const struct mstate *const cur_state = core->states.current_state;
	struct mstate *const next_state = core->states.next_state;
	
	//Cache the next actuation position;
	int32_t *const next_actuation_pos = next_state->actuation_positions;
	
	//Cache the actuators model arrays;
	struct actuator_model **actuators_models = ctrl->actuators_models;
	
	//Cache the movement builder; All temp fields are copied;
	struct computation_data cmp_data = core->cmp_data;
	
	
	/*
	 * Init;
	 */
	
	//Reset the next state's status;
	next_state->status = 0;
	
	
	/*
	 * Stage 0 : movement distances computation;
	 */
	
	//Compute movement distances;
	(*(ctrl->compute_distances))(cur_state, next_state, mv_distances);
	
	//The distances computation has updates the state's status;
	uint32_t status = next_state->status;
	
	//If the actuation position is not computed, invalidate the whole state;
	if (!(status & MSTATUS_ACTUATION_POS)) {
		status = 0;
	}
	
	
	/*
	 * 1 - durations bound computation :
	 * 	Each actuator's physical model computes the admissible duration interval for each axis;
	 */
	
	compute_durations(dimension, ctrl->actuators_models, mv_distances, cur_state, cmp_data.interval_array);
	
	
	/*
	 * 2 - interval merge :
	 * 	determines determine the best admissible interval, given the set of duration intervals;
	 * 	If authorised, will activate the distance correction node for all required actuators;
	 *
	 * 	If the interval can be reduced with other rules, true will be returned;
	 */
	
	bool reducing_enabled = merge_intervals(dimension, cmp_data.interval_array, cmp_data.final_interval);
	
	
	/*
	 * 	3 - distance_correction :
	 * 		for each axis, determines the closest distance from the original distance, that matches actuation constraints;
	 * 		returns true if a distance is modified;
	 */
	
	bool distances_corrected = correct_distances(dimension, actuators_models, cmp_data.interval_array,
												 cmp_data.final_interval->min, mv_distances);
	
	
	//If distances are corrected, invalidate the whole state;
	if (distances_corrected) {
		status = 0;
	}
	
	//If actuation position is not computed :
	if (!(status & MSTATUS_ACTUATION_POS)) {
		
		/*
		 *  4 - next position computation :
		 * 		determines the next actuation position, given the current state, and the distances array.
		 */
		
		compute_next_position(dimension, cur_state->actuation_positions, mv_distances, next_actuation_pos);
		
	}
	
	//If control position is not computed :
	if (!(status & MSTATUS_CONTROL_POS)) {
		
		/*
		 * 	5 -  geometric_model inversion :
		 *		If a distance has been corrected, will invert the geometric_model, to get control positions from actuation
		 *		positions.
		 */
		
		(*(ctrl->geometry->actuation_to_control))(next_actuation_pos, next_state->control_positions);
		
		
	}
	
	//Mark both positions computed;
	status |= MSTATUS_ACTUATION_POS || MSTATUS_CONTROL_POS;
	
	//Save the machines state;
	next_state->status = status;
	
	
	/*
	 *	6 - controller builder computation :
	 *		the controller computes its part of the movement builder; It may realise any computation it requires for
	 *		next stages;
	 */
	
	compute_builder(ctrl, cur_state, next_state, cmp_data.controller_data);
	
	
	//If the duration interval can be reduced :
	if (reducing_enabled) {
		
		/*
		 *	7 - duration interval reducing :
		 *		If the duration interval for the current movement is not a singleton, the machine controller applies
		 *		restrictions on this interval (based for ex on acceleration bounds, or jerk planning), until it becomes a
		 *		singleton.
		 *		If the duration interval is still not a singleton at the end of this stage, the maximal distance (lowest speed)
		 *		is selected;
		 */
		
		reduce_interval(ctrl, cur_state, cmp_data.controller_data, cmp_data.final_interval);
		
	}
	
	//Update the movement time;
	mvmt->time_to_dest = cmp_data.final_interval->min;
	
	
	/*
	 *	8 - controller state computation :
	 *		the controller computes its part of the movement builder; It may realise any computation it requires to compute
	 *		the next movement;
	 */
	
	compute_state(ctrl, cur_state, cmp_data.controller_data, next_state);
	
	//Update the current state;
	machine_states_accept(&core->states);
	
}


//--------------------------------------------------- Stages ---------------------------------------------------

/**
 * Stage 0 - inline;
 */


/**
 * Stage 1 - compute_durations : computes the duration interval for each actuator, given the distances array and the
 * 	current state;
 *
 * //TODO COMMENT;
 * @param controller : the current state of the machine;
 * @param dst : the time interval array to update;
 */

static void compute_durations(
	uint8_t dimension,
	struct actuator_model **actuators,
	const int16_t *distances, const struct mstate *const current_state,
	struct time_interval *dst) {
	
	//For each actuator :
	while (dimension--) {
		
		//Cache the actuator;
		struct actuator_model *instance = *actuators;
		
		//Cache the computation function;
		void (*const computation)(struct actuator_model *, int16_t, const struct mstate *,
								  struct time_interval *const) = instance->compute_duration_interval;
		
		//If the actuator has physical limitations :
		if (computation) {
			
			//Compute the duration interval;
			(*computation)(instance, *distances, current_state, dst);
			
		} else {
			//If the actuator has no physical limitations :
			
			//Simply set the destination interval to the largest one;
			*dst = time_interval_largest;
		}
		
		//Update all pointers;
		distances++, dst++, actuators++;
		
	}
	
}


/**
 * Stage 2 - merge_intervals : this functions will regroup all duration intervals in a single one, and if required,
 * 	choose a time, and activate distances corrections for relevant actuators;
 *
 * @param dimension : the machine dimension;
 * @param intervals : the duration interval array
 * @param dst_interval : the final duration interval;
 *
 * @return true if the interval is valid;
 */

static bool merge_intervals(
	const uint8_t dimension,
	struct time_interval *const intervals,
	struct time_interval *const dst_interval) {
	
	//Regroup intervals in a single one;
	struct time_interval final_interval = timer_interval_merge(dimension, intervals);
	
	//Cache the validity state;
	bool valid = final_interval.valid;
	
	//If the required movement didn't match with actuation physical limitations :
	if (!valid) {
		
		//Choose arbitrarily a duration, eventually the previous movement's duration;
		choose_duration(&final_interval, dst_interval->min);
		
		//Activate distance correction for relevant actuators;
		activate_corrections(dimension, final_interval.min, intervals);
		
	}
	
	//Save the time interval;
	*dst_interval = final_interval;
	
	//Return true if the interval is valid;
	return valid;
	
}


/**
 * Stage 3 - correct_distances : for each actuator with an invalid duration bound, recalculate the distance to be
 * 	conform to the duration;
 *
 * @param dimension : the machine dimension;
 * @param distances : the distances array;
 * @param controller : the
 * @param builder
 */

static bool correct_distances(
	uint8_t dimension,
	struct actuator_model **actuators,
	const struct time_interval *intervals,
	const float duration,
	int16_t *distances) {
	
	//The correction flag, set if almost one distance correction has been done;
	bool corrected = false;
	
	//For each axis :
	while (dimension--) {
		
		//Cache the actuator;
		struct actuator_model *instance = *actuators;
		
		//Cache the correction function;
		void (*const correction)
			(struct actuator_model *, const float, int16_t *const)=instance->compute_minimal_distance;
		
		//If the duration is invalid and the actuator supports distance adjustment :
		if ((!(intervals->valid)) && (correction)) {
			
			//Set the correction flag;
			corrected = true;
			
			
			//Recalculate the distance;
			(*correction)(instance, duration, distances);
			
		}
		
		//Update pointers;
		intervals++, actuators++, distances++;
		
	}
	
	//Return the correction status;
	return corrected;
	
}


/**
 * Stage 4 - compute_next_position :
 *
 * 	Sums distances to positions and store results in the destinations array;
 *
 * @param dimension : the number of axis;
 * @param position : the array of positions;
 * @param dists : the array of distances;
 * @param dests : the array of destinations;
 */

static void compute_next_position(
	uint8_t dimension,
	const int32_t *position,
	const int16_t *dists,
	int32_t *dests) {
	
	//For each actuator :
	while (dimension--) {
		
		//Set the destination to the sum of the current position and converted distance; Update pointers btw;
		*(dests++) = *(position++) + (int32_t) *(dists++);
		
	}
	
}


//--------------------------------------------------- Initialisation ---------------------------------------------------



/*
 * compute_state : updates the controller state, by executing each enabled builder computations;
 *
 * //TODO COMMENT;
 */

static void compute_builder(
	const struct mstate *const current_state,
	const struct mstate *const new_state,
	void *const controller_data
) {
	
	//For each computation :
	for (uint8_t cpt_id = 0; cpt_id < NB_BUILDER_CPTS; cpt_id++) {
		
		//Execute the computation;
		(*(builder_computations[cpt_id]))(current_state, new_state, controller_data);
		
	}
	
}


/*
 * mcontroller_determine_duration : determines the current movement's final duration window, evaluating all
 * constraints successively;
 * //TODO COMMENT;
 */


static void reduce_interval(
	const struct mstate *current_state,
	const void *controller_data,
	struct time_interval *duration_window) {
	
	//Create a cached copy of the provided time interval;
	struct time_interval final_interval = *(duration_window);
	
	//If the final interval is invalid :
	if (!final_interval.valid) {
		
		//Error, not supposed to happen;
		kernel_log_("machine_controller.c : mcontroller_correct_duration_window : the final interval is invalid;");
		
	}
	
	//If the time interval is already a singleton :
	if ((final_interval.max_exists) && (final_interval.min >= final_interval.max)) {
		
		//Nothing to do;
		return;
		
	}
	
	//Create a time interval that we will give to kinematic constraints; They will initialise it;
	struct time_interval constraint_interval = {0};
	
	//For each computation to realise :
	for (uint8_t cnst_id = 0; cnst_id < NB_KINEMATIC_CSTRS; cnst_id++) {
		
		//Determine the constraint's required duration window; constraint_interval will be re-initialised;
		(*(constraints[cnst_id]))(current_state, controller_data, &final_interval);
		
		//The constraint updates the provided interval to a non empty valid one;
		
		//Correct the duration window according to the new restriction;
		const bool singleton = time_interval_intersect(&final_interval, constraint_interval);
		
		//If the duration window is now a singleton :
		if (singleton) {
			
			//no more constraints to evaluate;
			break;
			
		}
		
	}
	
	//Now that the duration window is reduced to its minimal width, save it;
	*duration_window = final_interval;
	
}


/*
 * compute_state : updates the controller state, by executing each enabled builder computations;
 * //TODO COMMENT;
 */

static void compute_state(
	const struct mstate *const current_state,
	const struct computation_data *const cmp_data,
	struct mstate *const new_state) {
	
	
	//For each computation :
	for (uint8_t cpt_id = 0; cpt_id < NB_STATE_CPTS; cpt_id++) {
		
		//Execute the computation;
		(*(state_computations[cpt_id]))(current_state, cmp_data, new_state);
		
	}
	
}


//------------------------------------------------ Durations corrections -----------------------------------------------

/*
 * choose_duration : this function will choose an arbitrary duration, if the final interval is invalid;
 *
 * 	Depending on the interval, it will select the previous time, or the average of min and max;
 *
 * 	After choosing the time, it will re-initialise the interval to a safe, valid, singleton;
 * //TODO COMMENT;
 */

static void choose_duration(struct time_interval *const final_interval, const float previous_time) {
	
	//We must choose an arbitrary time;
	float time;
	
	//If a time interval is not provided, or if no actuator could provide a possible duration window (rare):
	if (!final_interval->max_exists) {
		
		//The movement will have the same duration as the previous one;
		time = previous_time;
		
	} else {
		//If a time interval exist, choose the average btw min and max bound;
		
		//Arbitrary choose the average of min and max;
		time = (final_interval->max + final_interval->min) / (float) 2;
		
	}
	
	//Save the time and update flags;
	*final_interval = (struct time_interval) {
		.max_exists = true,
		.max = time,
		.min = time,
		.valid = true,
		.empty = false,
	};
	
}


/*
 * activate_corrections : will traverse all time intervals, and activate the distance correction for the
 * 	invalid ones, or for those that do not contain the new time;
 * //TODO COMMENT;
 */

static void activate_corrections(
	const uint8_t dimension,
	const float time,
	struct time_interval *const intervals) {
	
	//For each actuator :
	for (uint8_t actuator_index = 0; actuator_index < dimension; actuator_index++) {
		
		//Cache the current interval pointer;
		struct time_interval *interval = intervals + actuator_index;
		
		//If the interval is empty, or if the final time is out of it, a correction is required;
		if ((!interval->valid) || (time < interval->min) || ((interval->max_exists) && (time > interval->max))) {
			
			//Set the interval to the required time, and mark it invalid for further correction;
			interval->valid = false;
			interval->min = time;
			
		}
		
	}
	
}


//------------------------------------------------ States query / switch -----------------------------------------------


/**
 * machine_states_accept : switches the couple of states;
 *
 * 	The next state becomes the current one, and the current one is invalidated and becomes the next one;
 *
 * @param states : the couple of machine states;
 */

static void machine_states_accept(struct mstates *const states) {
	
	//If the candidate state is s0 :
	if (states->current_is_s0) {
		
		//Assign s1 to the current, and s0 to the candidate;
		states->current_state = &(states->s1);
		states->next_state = &(states->s0);
		states->current_is_s0 = false;
		
	} else {
		//If the candidate state is s1 :
		
		//Assign s0 to the current, and s1 to the candidate;
		states->current_state = &(states->s0);
		states->next_state = &(states->s1);
		states->current_is_s0 = true;
		
	}
	
}

