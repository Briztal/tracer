/*
  distances_computer.h Part of TRACER

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

#ifndef TRACER_POSITION_COMPUTER_H
#define TRACER_POSITION_COMPUTER_H

#include <stdint.h>

#include "machine.h"

#include "kernel/computation/staged_execution.h"


/*
 * TODO DOC
 */

typedef struct {

	//The position target computer instance;
	void *instance;

	//Compute the position for the new machine state;
	cnode_state_t (*compute)(

		//The instance is provided; Write is allowed;
		void *instance,

		//Machine constants are provided. Read only;
		const machine_constants_t *machine,

		//The current state is provided. Read only;
		const machine_state_t *current_state,

		//The new custom state is provided. Write is allowed;
		void *new_custom_state,

		//The distances array to fill is provided. Write is allowed;
		int16_t *distances

	);

	//Notify the position computer that the previously provided position has been rejected;
	void (*reset)(void *instance);

	//Notify the position computer that the previously provided position has been accepted;
	void (*accept)(void *instance);

	//Delete the position computer. Required for cleanup;
	void (*delete)(void *instance);

} distances_computer_t;


//---------------------------------------- inline shortcuts ----------------------------------------

/*
 * Shortcut for distances computation stage;
 */

inline cnode_state_t distances_computer_compute(
	distances_computer_t *computer, const machine_constants_t *constants, const machine_state_t *current_state,
	void *const new_custom_state, int16_t *distances
) {

	//Compute the position and return the status;
	return (*(computer->compute))(computer->instance, constants, current_state, new_custom_state, distances);

}



#endif //TRACER_POSITION_COMPUTER_H
