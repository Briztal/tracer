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

/*
 * A machine comprises two states, the current, that is read only, and the candidate one that is currently
 * in computation, and can be modified;
 *
 * 	When the candidate state is accepted, it becomes the current state;
 */

typedef struct {

	//The current machine state;
	const machine_state_t *current_state;

	//The machine state in computation;
	machine_state_t *candidate_state;

	//The machine state identifier; set when "*candidate_state == s0";
	bool candidate_is_t0;

	//The first machine state;
	machine_state_t s0;

	//The second machine state;
	machine_state_t s1;

} machine_states_t;


//Accept the current machine state; Switches pointers;
inline void machine_states_accept(machine_states_t *states) {

	//If the candidate state is s0 :
	if (states->candidate_is_t0) {

		//Assign s0 to the current, and s1 to the candidate;
		states->current_state = &(states->s0);
		states->candidate_state = &(states->s1);

	} else {
		//If the candidate state is s1 :

		//Assign s1 to the current, and s0 to the candidate;
		states->current_state = &(states->s1);
		states->candidate_state = &(states->s0);

	}

}


//Return the current state;
inline const machine_state_t * machines_states_current_state(machine_states_t *states) {
	return states->current_state;
}


//Return the candidate state;
inline const machine_state_t * machines_states_candidate_state(machine_states_t *states) {
	return states->candidate_state;
}


#endif //TRACER_MACHINE_STATES_H
