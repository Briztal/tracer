/*
  mcore.h Part of TRACER

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

#ifndef TRACER_MACHINE_CORE_H
#define TRACER_MACHINE_CORE_H

#include <stdint.h>

#include <stdbool.h>

#include <stddef.h>

#include "time_interval.h"

#include "mcontroller.h"



//------------------------------------------------------ Movement ------------------------------------------------------

/*
 * A movement is represented by a set of coordinates and a time to complete;
 */

struct movement {
	
	//The distances coordinates array;
	int16_t *const actuation_distances;
	
	//The number of microseconds till position must be reached;
	float time_to_dest;
	
};


//------------------------------------------- Machine state -------------------------------------------

/*
 * A machine state contains all data that describes the real time state of the machine;
 *
 * 	It comprises a mandatory part, that presents the actuation layer's current state, and a custom part,
 * 	that is left to the controller's implementation;
 */

struct mstate {
	
	//States are created independently from any core or controller. Their dimension must be check at init;
	const uint8_t dimension;
	
	//The status of the computation; Each bit marks one computation stage realised; Bit 31 is reserved;
	uint32_t status;
	
	//Actuation positions. Actuators commanded on an integer base;
	int32_t *const actuation_positions;
	
	//Control positions. Expressed in float;
	float *const control_positions;
	
};


/*
 * The only reserved bit of machine state's status is bit 31, that represents the validity of actuation and control
 * positions;
 */

#define MSTATUS_ACTUATION_POS ((uint32_t) ((uint32_t) 1 << 30))

#define MSTATUS_CONTROL_POS ((uint32_t) ((uint32_t) 1 << 31))


//------------------------------------------- Machine states -------------------------------------------

/*
 * A machine comprises two states, the current, that is read only, and the candidate one that is currently
 * in comput, and can be modified;
 *
 * 	When the candidate state is accepted, it becomes the current state;
 */

struct mstates {
	
	//The current machine state;
	const void *current_state;
	
	//The machine state in comput;
	void *next_state;
	
	//The machine state identifier; set when "*next_state == s0";
	bool current_is_s0;
	
	//The first machine state;
	struct mstate *s0;
	
	//The second machine state;
	struct mstate *s1;
	
};


//------------------------------------------------- Movement computation -------------------------------------------------

/*
 * Movements are computed step by step, and temporary data that is not related to a machine state
 * 	is stored in the movement builder.
 */

struct computation_data {
	
	//The time interval array, containing the duration window for each actuator during the comput;
	struct time_interval *const interval_array;
	
	//The duration interval that satisfies all constraints;
	struct time_interval *final_interval;
	
	//The controller builder. Can contain whatever the controller decides;
	void *controller_data;
	
};


//------------------------------------------------- Machine core -------------------------------------------------

/*
 * A machine controller contains the following :
 */

struct mcore {
	
	/*
	 * Constants
	 */
	
	//The machine dimension, constant;
	const uint8_t dimension;
	
	
	/*
	 * External control;
	 */
	
	//The machine's controller, owned, mutable;
	struct mcontroller *controller;
	
	
	/*
	 * Machine states;
	 */
	
	//Machine states, owned, mutable;
	struct mstates states;
	
	
	/*
	 * Computation;
	 */
	
	//The movement builder, owned, mutable;
	struct computation_data cmp_data;
	
	/*
	 * State;
	 */
	
	bool ready;
	
	/*
	 * Result;
	 */
	
	//The movement that we currently compute; References the distances array and the duration;
	struct movement *movement;
	
	
};


//Initialise a machine core, providing a machine controller, two states, and a ref to the controller computation struct;
void mcore_initialise(
	struct mcore *core,
	struct mcontroller *controller,
	struct mstate *s0,
	struct mstate *s1,
	void *controller_computation_data
);

//Set the location where the computed movement should be stored;
void mcore_set_movement_dst(struct mcore *, struct movement *);

//Compute the movement;
bool mcore_compute_movement(struct mcore *);


#endif //TRACER_MACHINE_CORE_H
