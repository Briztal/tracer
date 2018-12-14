/*
  solving.h Part of TRACER

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


#ifndef TRACER_SOLVING_H
#define TRACER_SOLVING_H

#include <stddef.h>


/**
 * A state is a memory representation. It contains variables that must be computed.
 *
 * 	It is composed of state variables and abstract variables.
 *
 * 	A state can have several states :
 * 	- empty : state and abstract variables are invalid;
 * 	- partial : state variables are valid, abstract are not;
 * 	- valid : both state and abstract are valid;
 *
 *
 * A controller is a set of determined values. They are provided to the solver.
 *
 * 	They intend to control the way the solver works;
 *
 *
 * Solving iteration :
 *
 * 	known data :
 * 		- current state, complete;
 *
 * 	data to determine :
 * 		- next state, complete;
 *
 * 	temporary data :
 * 		- current target state, partial;
 *
 *
 * 	process :
 * 		- use the solver to compute the current target state, using the current state;
 * 		- get the next state's state variables, using the current target state;
 * 		- complete the next state, using the current state;
 *
 *
 * The second step of the process composes the closed loop. It is when the state is updated, after the target state
 * has been used;
 * In physical systems, there is a delay between the time target state is computed, and next state is updated;
 * The algorithm should be then reformulated like below :
 *
 *
 * Solving iteration :
 *
 * 	known data :
 * 		- current state, complete;
 * 		- current target state, partial;
 *
 * 	data to determine :
 * 		- next state, complete;
 * 		- next target state, partial;
 *
 * 	process :
 * 		- get the next state's state variables, using the current target state;
 * 		- complete the next state, using the current state;
 * 		- use the solver to determine the next target state, using the next state;
 *
 * 	In this algorithm, current and next target have separate lifetimes. Both can occupy the same memory space.
 * 	current and next states, however, share a part of their lifetime, and must occupy different memory spaces.
 */

struct solver_ops {
	
	//Update the next state. If null, the target state will be used as next state;
	void (*update_state)(void *next_state);
	
	//The function to compute the state's abstract variables;
	void (*complete_state)(void *incomplete_state, const void *complete_state);
	
	//The solver;
	void (*compute_target)(
		
		//Control data;
		void *ctrl,
		
		//The ref of the complete state state, constant;
		const void *state,
		
		//The ref of the target state, to update partially, mutable;
		void *target
		
		//The ref of the transition data struct, mutable;
		//void *transition_tmp,
	
	);
	
};



struct solver {
	
	//Control data;
	void *s_ctrl;
	
	//Target state, partial;
	void *s_target;
	
	//Current state, complete;
	void *s_current;
	
	//Next state, to compute;
	void *s_next;
	
	//The transition data;
	//void *s_tmp;
	
	//The operation struct;
	struct solver_ops *s_ops;
	
};



//Get the current state of a solving unit;
inline const void *solver_get_state(struct solver *s) {
	
	//Return the current state of the appropriate unit;
	return s->s_current;
	
}


//Get the current state of a solving unit;
inline const void *solver_get_target(struct solver *s) {
	
	//Return the current state of the appropriate unit;
	return s->s_target;
	
}


//Actualise the state of a solving unit;
void solver_iterate(struct solver *env);


#endif //TRACER_SOLVING_H

