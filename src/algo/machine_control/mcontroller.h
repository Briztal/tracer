/*
  mcontroller.h Part of TRACER

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

#ifndef TRACER_MACHINE_CONTROLLER_H
#define TRACER_MACHINE_CONTROLLER_H

#include <stdbool.h>

#include "time_interval.h"


#include "mcore.h"
#include "actuation.h"
#include "geometry.h"



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


//------------------------------------------------- Machine Controller -------------------------------------------------

/*
 * A machine controller is composed of :
 * 	- a dimension;
 * 	- a geometric model;
 * 	- a distances dist_computer;
 * 	- an actuator model;
 * 	- an array of controller builder computations;
 * 	- an array of kinematic constraints
 * 	- an array of controller state computations;
 * 	- data to determine whether the controller is ready to compute movements;
 */

struct mcontroller {
	
	//---------------------------------------------- Dimension ----------------------------------------------
	
	//The machine dimension. Constant;
	const uint8_t dimension;
	
	//---------------------------------------------- geometry ----------------------------------------------
	
	//The machine's geometric_model, mutable ref;
	struct geometric_model *geometry;
	
	
	//---------------------------------------------- Distance computation ----------------------------------------------
	
	//The distance dist_computer;
	struct distance_computer *dist_computer;
	
	
	//----------------------------------------------- Actuators models ----------------------------------------------
	
	//The actuation physics array, mutable ref;
	struct actuator_model **const actuators_models;
	
	
	//----------------------------------------------- Builder comput ----------------------------------------------
	
	//The number of controller builder computations;
	const uint8_t nb_builder_cpts;
	
	//The array of builder computations;
	builder_cpt *const builder_cpts;
	
	
	//---------------------------------------------- Kinematic constraints ---------------------------------------------
	
	//The number of kinematic constraints;
	const uint8_t nb_kinematic_cnsts;
	
	//The array of kinematic constraints;
	kinematic_cnst *const kinematic_cnsts;
	
	
	//------------------------------------------------ State comput -----------------------------------------------
	
	//The number of controller state computations;
	const uint8_t nb_state_cpts;
	
	//The array of state computations;
	state_cpt  *const state_cpts;
	
	
	//------------------------------------------------ Execution flag -----------------------------------------------
	
	//Number of uninitialised actuator models;
	uint8_t nb_uninitialised_actuators;
	
	//Number of uninitialised builder computations;
	uint8_t nb_uninitialised_builder_cpts;
	
	//Number of uninitialised kinematic constraints;
	uint8_t nb_uninitialised_kinematic_cnsts;
	
	//Number of uninitialised state computations;
	uint8_t nb_uninitialised_state_cpts;
	
	//A flag, set if all data is properly initialised;
	bool ready;
	
};


//--------------------------------------------------- Initialisation ---------------------------------------------------

//Set the geometric model;
void mcontroller_set_geometric_model(struct mcontroller *ctrl, struct geometric_model *geometry);

//Set the distance computer;
void mcontroller_set_distance_computer(struct mcontroller *ctrl, struct distance_computer *cptr);

//Update the model of an actuator;
void mcontroller_init_actuator_model(struct mcontroller *ctrl, uint8_t actuator_id, struct actuator_model *model);

//Set a builder computation function;
void mcontroller_init_builder_cmp(struct mcontroller *ctrl, uint8_t cmp_id, builder_cpt computation);

//Set a kinematic constraint function;
void mcontroller_init_kinematic_cnst(struct mcontroller *ctrl, uint8_t cnst_id, kinematic_cnst constraint);

//Set a state computation function;
void mcontroller_init_state_cmp(struct mcontroller *ctrl, uint8_t cmp_id, state_cpt computation);



#endif //TRACER_MACHINE_CONTROLLER_H
