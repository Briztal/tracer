/*
  planner.c Part of TRACER

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



#include <stdlib.h>
#include <string.h>
#include <dmz/stdmem.h>

#include "planner.h"

//tODO create transition : creates a transition and a singularity;
//tODO create transition : creates a transition and a singularity;
//tODO create transition : creates a transition and a singularity;
//tODO create transition : creates a transition and a singularity;
//tODO create transition : creates a transition and a singularity;
//tODO create transition : creates a transition and a singularity;
//tODO create transition : creates a transition and a singularity;


//TODO LINK SINGLARTIES PROPERLY;
//TODO LINK SINGLARTIES PROPERLY;
//TODO LINK SINGLARTIES PROPERLY;
//TODO LINK SINGLARTIES PROPERLY;
//TODO LINK SINGLARTIES PROPERLY;
//TODO LINK SINGLARTIES PROPERLY;
//TODO LINK SINGLARTIES PROPERLY;
//TODO LINK SINGLARTIES PROPERLY;
//TODO LINK SINGLARTIES PROPERLY;
//TODO LINK SINGLARTIES PROPERLY;

void planner_add_elmt(struct planner *pl, struct planner_elmt *elmt) {

	//Cache the planner size.
	const uint32_t size = pl->csts.p_size;
	
	/*
	 * Foreplay : In all cases, a transition will have to be created, and in all cases, the element's trajectory
	 * 	must be evaluated in its first point.
	 *
	 * 	A temporary transition will be created. In case of hook mode, t1 will be assigned to it. In case of
	 * 	no hook, t2 will be assigned to it, to avoid unnecessary dealloc-alloc;
	 */
	
	//Create a transition, and a singularity, to contain the element's trajectory's first point;
	struct transition *tmp_trans = create_transition();
	
	//Cache the transition's singularity;
	struct singularity *elmt_sing = tmp_trans->t_sing;
	
	//Cache the element's trajectory;
	struct trajectory *elmt_traj = elmt->pe_traj;
	
	//Evaluate the trajectory's first point and store the result in the cached singularity struct;
	trajectory_evaluate(elmt_traj, elmt_traj->t_imin, elmt_sing->s_pos, size);
	
	
	/*
	 * In this function, three transition will be involvedt :
	 * - t0, the last transition of the list. Must be a stop transition. May not exist if the planner is
	 * 	already stopped;
	 * - t1 : the transition starting the process of @elmt; May be equal to t0 if no hook mode is required;
	 * - t2 : the transition terminating the process of @elmt; Must be created, and will be a stop transition;
	 */
	struct transition *t0, *t1, *t2;
	
	/*
	 * Two particular cases must be handled :
	 * 	- the planner is stopped, and t0 must be created. Signaled by @starting;
	 * 	- No hook mode is required. t0 and t1 will have the same value. Signaled by @no_hook;
	 */
	
	bool starting = false, no_hook = false;
	
	/*
	 * Determination of t0, t1 and t2;
	 */
	
	//Cache the first transition;
	struct transition *first_t = pl->p_trans;
	
	//If the list is empty :
	if (!first_t) {
		
		//Signal the start of the planner;
		starting = true, no_hook = false;
		
		//A transition must be created, in order to start the planner;
		pl->p_trans = t0 = create_transition();
		
		//Cache t0's singularity,
		struct singularity *sing = t0->t_sing;
		
		//Reset the singularity's position, on which we have no information;
		memset(sing->s_pos, 0, sizeof(float) * size);
		
		//Reset the singularity's input direction, on which we have no information;
		memset(sing->s_dir0, 0, sizeof(float) * size);
		
		//At planner startup, hook mode is forced. t1 is assigned to tmp_trans;
		t1 = tmp_trans;
		
		//Insert t1 after t0;
		list_concat((struct list_head *) t0, (struct list_head *) t1);
		
		//A new transition must be created for t2;
		t2 = create_transition();
		
		//t0 will cause a transition to hook mode, with no information on directions;
		assign_to_hook(t0, t1, false);
		
		//TODO A ZERO TO ZERO SINGULARITY IS POTENTIALLY REVALIDATED. LINK SINGULARITIES PROPERLY
		//TODO A ZERO TO ZERO SINGULARITY IS POTENTIALLY REVALIDATED. LINK SINGULARITIES PROPERLY
		//TODO A ZERO TO ZERO SINGULARITY IS POTENTIALLY REVALIDATED. LINK SINGULARITIES PROPERLY
		//TODO A ZERO TO ZERO SINGULARITY IS POTENTIALLY REVALIDATED. LINK SINGULARITIES PROPERLY
		//TODO A ZERO TO ZERO SINGULARITY IS POTENTIALLY REVALIDATED. LINK SINGULARITIES PROPERLY
		//TODO A ZERO TO ZERO SINGULARITY IS POTENTIALLY REVALIDATED. LINK SINGULARITIES PROPERLY
		//TODO A ZERO TO ZERO SINGULARITY IS POTENTIALLY REVALIDATED. LINK SINGULARITIES PROPERLY
		//TODO A ZERO TO ZERO SINGULARITY IS POTENTIALLY REVALIDATED. LINK SINGULARITIES PROPERLY
		//TODO A ZERO TO ZERO SINGULARITY IS POTENTIALLY REVALIDATED. LINK SINGULARITIES PROPERLY
		//TODO A ZERO TO ZERO SINGULARITY IS POTENTIALLY REVALIDATED. LINK SINGULARITIES PROPERLY
		
		//t1 and t2 init, and t2 insertion are made in all cases;
		
		
	} else {
		
		//t0 is the first transition's predecessor;
		t0 = first_t->t_head.prev;
		
		//The distance between t0's singularity point and the trajectory's init point must be evaluated;
		float hook_dist = compute_cost(t0->t_sing->s_pos, elmt_sing);
		
		//If the hook distance is too high :
		if (hook_dist > pl->csts.p_dmax) {
			
			//Hook will be enabled; t1 is assigned to tmp_trans;
			t1 = tmp_trans;
			
			//Insert t1 after t0;
			list_concat((struct list_head *) t0, (struct list_head *) t1);
			
			//A new transition must be created for t2;
			t2 = create_transition();
			
			//t0 will cause a transition to hook mode, with information on directions;
			assign_to_hook(t0, t1, true);
			
			//t1 and t2 init, and t2 insertion are made in all cases;
			
		} else {
			
			//If the hook distance is too low, hook mode is disabled;
			
			//t0 and t1 share the same value;
			t1 = t0;
			
			//t2 referenced the firstly allocated transition. The computed singularity point will be overwritten;
			t2 = tmp_trans;
			
		}
		
	}
	
	//Insert t2 after t1;
	list_concat((struct list_head *) t1, (struct list_head *) t2);
	
	//Assign t1 to trajectory, and t2 to stop;
	assign_to_traj(t1, t2);
	

}


static bool planner_rm_elmt(struct planner *pl) {
	
	//Cache the first transition;
	struct planner_elmt *elmt = pl->p_elmts;
	
	//If the list is empty :
	if (!elmt) {
		
		//TODO LOG;
		
		//Fail;
		return false;
		
	}
	
	//Remove the first element; list_head composes the planner element, which legitimates casts;
	list_remove_ref_next((struct list_head *) elmt, (struct list_head **) &pl->p_elmts);
	
	//Mark the element to be recycled;
	list_concat_ref((struct list_head *) elmt, (struct list_head **) &pl->p_old_elmts);
	
}

//TODO FACTORY MUST BE CHECKED AT STARTUP !
//TODO FACTORY MUST BE CHECKED AT STARTUP !
//TODO FACTORY MUST BE CHECKED AT STARTUP !
//TODO FACTORY MUST BE CHECKED AT STARTUP !
//TODO FACTORY MUST BE CHECKED AT STARTUP !
//TODO FACTORY MUST BE CHECKED AT STARTUP !
//TODO FACTORY MUST BE CHECKED AT STARTUP !
//TODO FACTORY MUST BE CHECKED AT STARTUP !
//TODO FACTORY MUST BE CHECKED AT STARTUP !


/**
 * planner_rm_transition : removes the first transition of the list.
 * @param pl : the planner to update;
 * @return false if an error occurred;
 */

static bool planner_rm_transition(struct planner *pl) {
	
	//Cache the first transition;
	struct irregular_point *tr = pl->p_trans;
	
	//If the list is empty :
	if (!tr) {
		
		//TODO LOG;
		
		//Fail;
		return false;
		
	}
	
	//Remove the transition; list_head composes transition, which legitimates casts;
	list_remove_ref_next((struct list_head *) tr, (struct list_head **) &pl->p_trans);
	
	//Cache the factory;
	struct transition_factory *factory = pl->p_factory;
	
	//Call the deletion method;
	(*(factory->delete_transition))(factory, tr);
	
	//Complete;
	return true;
	
}


static bool planner_enter_mode(struct planner *pl, const enum planner_state next_state) {
	
	//Cache the current state;
	const enum planner_state cur_state = pl->p_state;
	
	
	/*
	 * Transition verification;
	 *
	 * Forbidden transitions are :
	 * - X -> X (X a state);
	 * - HOOK -> STOPPED;
	 * - STOPPED -> TRAJECTORY;
	 */
	
	if ((cur_state == next_state) ||
		((cur_state == PLANNER_HOOK_MODE) && (next_state == PLANNER_STOP_MODE)) ||
		((cur_state == PLANNER_STOP_MODE) && (next_state == PLANNER_TRAJECTORY_MODE))) {
		
		//TODO LOG;
		
		//Fail;
		return false;
		
	}
	
	//Cache return value;
	bool ret = true;
	
	//Optimisation;
	bool b = cur_state == PLANNER_TRAJECTORY_MODE;
	
	//The elements list must be altered only if the current mode is trajectory mode.
	if (b) {
		
		ret &= planner_rm_elmt(pl);
	}
	
	//The transition list must be altered only if one of states is trajectory mode;
	if (b || (next_state == PLANNER_TRAJECTORY_MODE)) {
		
		ret &= planner_rm_transition(pl);
	}
	
	//Update the state;
	pl->p_state = next_state;
	
	//Return true only if all steps succeeded.
	return ret;
	
	
}


static bool trajectory_get_point(struct planner *pl, const float pos_array[], float dst_array[], size_t size);



static bool hook_get_point(struct planner *pl, const float pos_array[], float dst_array[], size_t size);


