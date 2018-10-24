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
#include "geometry.h"
#include "actuation.h"



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
	
	//States are created independently from any core. Their dimension must be check at init;
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



//---------------------------------------------- Distance computation ----------------------------------------------

/*
 * The controller comprises a data structure whose goal is to provide the distance target for the next
 * 	movement.
 *
 * 	During this computation, it has access to the current state, that it can't alter, and to the current state,
 * 	that it can modify, to pre-compute some data, computations that it will signal in the state's status;
 *
 * 	It also has access to the array where distances must be stored;
 */

struct distance_computer {
	
	//The function that will compute movement distances;
	bool (*compute_distances)(
		struct distance_computer *,
		const struct mstate *current_state,
		struct mstate *new_state,
		int16_t *distances);
	
};


//------------------------------------------------ Builder computations ------------------------------------------------

/*
 * Extra computations may be required, depending on the controller's complexity level.
 *
 * 	A controller comput is a function that takes current and new states (consts), makes calculations, and
 * 	saves them in the controller part of the movement builder;
 *
 * 	The controller part of the movement builder is not defined, and completely depends on the implementation;
 *
 * 	Computations can be disabled.
 */

typedef void (*builder_cpt)(const struct mstate *const current_state,
							const struct mstate *const new_state,
							void *const const_builder);


//------------------------------------------------ Kinematic constraints -----------------------------------------------

/*
 * Kinematic constraints are functions that restrict the duration interval of a movement;
 *
 *	They are stored in link lists, and can be disabled, by clearing their flag.
 *
 *	They are composed of a function, that takes the machine's current state, and custom movement data, and returns
 *	an indicative time window for the movement.
 *
 *	Constraints are indicative, if the duration window they provide does not intersect with the mandatory time window,
 *	determined by the actuation layer and all more prioritary constraints, the appropriate bound of the mandatory time
 *	window is selected.
 */

typedef void (*kinematic_cnst)(const struct mstate *current_state,
							   const void *controller_data,
							   struct time_interval *duration_window);


//---------------------------------------------------- State updates ---------------------------------------------------

/*
 * Extra state fields may be required, depending on the controller's complexity level.
 *
 * 	A controller state comput is a function that takes current and new states (consts), makes calculations, and
 * 	saves them in the controller part of the movement builder;
 *
 * 	The controller part of the movement builder is not defined, and completely depends on the implementation;
 *
 * 	Computations can be disabled.TODO
 */

typedef void (*state_cpt)(const struct mstate *const current_state,
						  const struct computation_data *const const_builder,
						  struct mstate *const new_state);



//------------------------------------------------- Machine core -------------------------------------------------

/*
 * A machine controller contains the following :
 */

struct mcore {
	
	
	/*
	 * Constants
	 */
	
	//The machine dimension. Constant;
	const uint8_t nb_axis;
	
	//The number of controller builder computations;
	const uint8_t nb_builder_cpts;
	
	//The number of kinematic constraints;
	const uint8_t nb_kinematic_cnsts;
	
	//The number of controller state computations;
	const uint8_t nb_state_cpts;
	
	
	
	/*
	 * Computation model
	 */
	
	//The distances computer; Mutable.
	struct distance_computer *dist_computer;
	
	//The machine's geometric_model, constant ref;
	struct geometric_model *const geometry;
	
	//The actuation physics array, mutable ref;
	struct actuator_model **const actuators_models;
	
	//The array of builder computations;
	builder_cpt *const builder_cpts;
	
	//The array of kinematic constraints;
	kinematic_cnst *const kinematic_cnsts;
	
	//The array of state computations;
	state_cpt  *const state_cpts;
	
	
	
	/*
	 * Computation data;
	 */
	
	//Machine states, owned, mutable;
	struct mstates states;
	
	//The movement builder, owned, mutable;
	struct computation_data cmp_data;
	
	//The movement that we currently compute; References the distances array and the duration;
	struct movement *movement;
	
	
};


//Initialise a machine core, providing a machine controller, two states, and a ref to the controller computation struct;
void mcore_initialise(
	struct mcore *core,
	struct mstate *s0,
	struct mstate *s1,
	void *controller_computation_data
);

//Set the location where the computed movement should be stored;
void mcore_set_movement_dst(struct mcore *, struct movement *);

//Compute the movement;
bool mcore_compute_movement(struct mcore *);


#endif //TRACER_MACHINE_CORE_H
