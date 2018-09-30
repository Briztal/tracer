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



//--------------------------------------------------- Geometric model --------------------------------------------------

/*
 * The geometric_model is the relation between control coordinates and actuation coordinates.
 *
 *	It achieves conversion in both directions;
 *
 * Each geometric_model implementation will be composed of the basic geometric_model struct, that only comprises
 * 	a pointer to the conversion function;
 */

struct geometric_model {
	
	//A flag, determining if the geometric_model is regular, ie it transforms lines into lines;
	//If the geometric_model is not regular, extra computation will be required for distances determination;
	bool regular;
	
	//The pointer to the function used to convert control position to actuation position;
	void (*const control_to_actuation)(const float *const control, int32_t *const actuation);
	
	//The pointer to the function used to convert actuation position to control position;
	void (*const actuation_to_control)(const int32_t *const actuation, float *const control);
	
};


//--------------------------------------------------- Actuator model ---------------------------------------------------

/*
 * Actuators have physical limitations, that we must absolutely respect, otherwise, actuators or the machine could
 * 	be damaged;
 *
 * 	An actuator physics model is an interface, containing an instance and two functions :
 * 	- one to compute the largest acceptable duration interval for a movement of a given distance, given
 * 		the current state of the machine;
 * 	- one to compute a distance that matches physical limitation, for a movement of a given time, given the current
 * 		state of the machine state and the target distance;
 *  - A supplementary flag can be used to signal that the distance on this axis can be adjusted to comply with
 *  	physical limitations;
 */

struct actuator_model {
	
	/*
	 * The function to compute the time interval; takes following arguments;
	 * 	This function pointer can be null; If so, the duration interval will be set to its maximum;
	 */
	void (*const compute_duration_interval)(
		
		//The actuator physics instance;
		struct actuator_model *instance,
		
		//The actuator's movement distance;
		int16_t distance,
		
		//The machine's current state, constant;
		const struct mstate *current_state,
		
		//The location where to store the duration window, mutable;
		struct time_interval *const duration_window);
	
	
	/*
	 * The function to compute the minimal distance to fit duration requirements;
	 * 	This function pointer can be null; If so, no distance correction will happen;
	 */
	void (*const compute_minimal_distance)(
		
		//The actuator physics instance, mutable;
		struct actuator_model *instance,
		
		//The movement's duration, constant;
		const float movement_duration,
		
		//The pointer referencing the distance target. Must be updated with the calculated distance, mutable;
		int16_t *const distance_target);
	
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
 * 	- a distances computer;
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
	const struct geometric_model *geometry;
	
	
	//---------------------------------------------- Distance computation ----------------------------------------------
	
	//The function that will compute movement distances;
	bool (*compute_distances)(const struct mstate *current_state, struct mstate *new_state, int16_t *distances);
	
	
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

//Update the model of an actuator;
void mcontroller_init_actuator_model(struct mcontroller *ctrl, uint8_t actuator_id, struct actuator_model *model);

//Set a builder computation function;
void mcontroller_set_builder_cmp(struct mcontroller *ctrl, uint8_t cmp_id, builder_cpt computation);

//Set a kinematic constraint function;
void mcontroller_set_kinematic_cnst(struct mcontroller *ctrl, uint8_t cnst_id, kinematic_cnst constraint);

//Set a state computation function;
void mcontroller_set_state_cmp(struct mcontroller *ctrl, uint8_t cmp_id, state_cpt computation);



#endif //TRACER_MACHINE_CONTROLLER_H
