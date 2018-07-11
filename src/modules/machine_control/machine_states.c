/*
  machine_states.h Part of TRACER

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


#include "machine_states.h"

#include <stdbool.h>

#include <stddef.h>
#include <kernel/kernel.h>
#include <string.h>


//------------------------------------------------- State Init - cleanup -----------------------------------------------

/*
 * machine_state_initialise : allocates spaces for arrays, initialises them to 0, and transfer the ownership to the
 * 	provided state;
 */

void machine_state_initialise(machine_state_t *const state, const uint8_t dimension, void *const controller_state) {

	//Calculate sizes;
	size_t f_size = sizeof(float) * (size_t)dimension;
	size_t i_size = sizeof(int32_t) * (size_t)dimension;

	//Allocate space for actuation positions;
	int32_t *const actuation_positions = kernel_malloc(i_size);

	//Allocate space for actuation speeds;
	float *const actuation_speeds = kernel_malloc(i_size);

	//Allocate space for control positions;
	float *const control_positions = kernel_malloc(i_size);

	//Initialise all arrays to 0;
	memset(actuation_positions, 0, i_size);
	memset(actuation_speeds, 0, f_size);
	memset(control_positions, 0, f_size);

	//Create an initializer for the state;
	machine_state_t init = {

		//Transfer the ownership of all arrays;
		.actuation_positions = actuation_positions,
		.actuation_speeds = actuation_speeds,
		.control_positions = control_positions,

		//Transfer the ownership of the controller state;
		.controller_state = controller_state,

	};

	//Initialise the sate;
	memcpy(state, &init, sizeof(machine_state_t));

}

/*
 * machine_state_delete : frees all arrays of the provided state.
 * 	The controller state part is supposed deleted;
 */

void machine_state_delete(machine_state_t *const state) {

	//Delete the triplet of arrays;
	kernel_free(state->actuation_positions);
	kernel_free(state->actuation_speeds);
	kernel_free(state->control_positions);

	//The controller state must have been deleted;

}




//------------------------------------------------ States Init - cleanup -----------------------------------------------

/*
 * machine_states_initialise : Initialises states, and go in a safe state;
 *
 * The machine states composes another struct, so memory allocation is not required;
 */

void machine_states_initialise(machine_states_t *const states, const uint8_t dimension,  void *const controller_state0, void *const controller_state1) {

	//Initialise s0;
	machine_state_initialise(&states->s0, dimension, controller_state0);

	//Initialise s1;
	machine_state_initialise(&states->s0, dimension, controller_state0);


	//Assign s0 to the current, and s1 to the candidate;
	states->current_state = (const machine_state_const_t *) &(states->s0);
	states->candidate_state = &(states->s1);
	states->candidate_is_s0 = false;

}

/*
 * machine_states_delete : deletes the couple of states;
 */
void machine_states_delete(machine_states_t *states) {

	//Delete s0 and s1;
	machine_state_delete(&states->s0);
	machine_state_delete(&states->s1);

}


//------------------------------------------------ States query / switch -----------------------------------------------

//Accept the current machine state; Switches pointers;
void machine_states_accept(machine_states_t *const states) {

	//If the candidate state is s0 :
	if (states->candidate_is_s0) {

		//Assign s0 to the current, and s1 to the candidate;
		states->current_state = (const machine_state_const_t *) &(states->s0);
		states->candidate_state = &(states->s1);
		states->candidate_is_s0 = false;



	} else {
		//If the candidate state is s1 :

		//Assign s1 to the current, and s0 to the candidate;
		states->current_state = (const machine_state_const_t *) &(states->s1);
		states->candidate_state = &(states->s0);
		states->candidate_is_s0 = true;


	}

}


//Return the current state;
const machine_state_const_t *const machines_states_current_state(const machine_states_t *const states) {
	return states->current_state;
}


//Return the candidate state;
const machine_state_t *const machines_states_candidate_state(const machine_states_t *const states) {
	return states->candidate_state;
}

