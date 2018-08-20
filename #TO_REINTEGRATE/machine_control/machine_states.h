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

#ifndef TRACER_MACHINE_STATES_H
#define TRACER_MACHINE_STATES_H

#include "machine.h"


//------------------------------------------- Machine state -------------------------------------------

/*
 * A machine state contains all data that describe the real time state of the machine;
 *
 * 	It comprises a mandatory part, that presents the actuation layer's current state, and a custom part,
 * 	that is left to the controller's implementation;
 */

typedef struct {

	//Actuation positions. Actuators commanded on an integer base;
	int32_t *const actuation_positions;

	//Actuation speeds. Time is expressed in float, so are speeds;
	float *const actuation_speeds;

	//Control positions. Expressed in float;
	float *const control_positions;

	//The controller's state; Can contain whatever the controller decides;
	void *const controller_state;

} machine_state_t;


/*
 * Sometimes, some members can't be edited. Following structs provides restricted access to some members;
 */
typedef struct {

	int32_t *const actuation_positions;
	const float *const actuation_speeds;
	float *const control_positions;
	const void *const controller_state;

} machine_state_p_t;


typedef struct {

	const int32_t *const actuation_positions;
	const float *const actuation_speeds;
	const float *const control_positions;
	void *const controller_state;

} machine_state_l_t;


typedef struct {

	const int32_t *const actuation_positions;
	const float *const actuation_speeds;
	const float *const control_positions;
	const void *const controller_state;

} machine_state_const_t;



//--------------------------------------------------- Init - cleanup ---------------------------------------------------

//Initialise the states manager. Composes another struct, so not allocated directly;
void machine_state_initialise(machine_state_t *state, uint8_t dimension, void *controller_state);

//Delete the states manager;
void machine_state_delete(machine_state_t *state);



//------------------------------------------- Machine state -------------------------------------------

/*
 * A machine comprises two states, the current, that is read only, and the candidate one that is currently
 * in comput, and can be modified;
 *
 * 	When the candidate state is accepted, it becomes the current state;
 */

typedef struct {

	//The current machine state;
	const machine_state_const_t *current_state;

	//The machine state in comput;
	machine_state_t *candidate_state;

	//The machine state identifier; set when "*candidate_state == s0";
	bool candidate_is_s0;

	//The first machine state;
	machine_state_t s0;

	//The second machine state;
	machine_state_t s1;

} machine_states_t;


//--------------------------------------------------- Init - cleanup ---------------------------------------------------

//Initialise the states manager. Composes another struct, so not allocated directly;
void machine_states_initialise(machine_states_t *states, uint8_t dimension,
							   void *controller_state0, void *controller_state1);

//Delete the states manager;
void machine_states_delete(machine_states_t *states);


//------------------------------------------------ States query / switch -----------------------------------------------

//Accept the current machine state; Switches pointers;
void machine_states_accept(machine_states_t *states);

//Return the current state;
const machine_state_const_t *const machines_states_current_state(const machine_states_t *states);

//Return the candidate state;
const machine_state_t *const machines_states_candidate_state(const machine_states_t *states);


#endif //TRACER_MACHINE_STATES_H
