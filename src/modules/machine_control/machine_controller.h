/*
  machine_controller.h Part of TRACER

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

#include "machine.h"

#include "machine_states.h"

#include "movement_builder.h"


//---------------------------- Target distances computation ----------------------------

/*
 * The machine is controlled by providing algebraic distances, that actuators must move of;
 *
 * 	This computation can be realised in many ways, and can eventually be changed during the machine's life cycle;
 */

/*
 * To compute actuation distances, access is given to the following data :
 */

typedef struct {

	//The distances computer's instance;
	void *const instance;

	//Machine constants (dimension + geometry). Read only;
	const machine_constants_t *const machine;

	//The current machine state. Read only;
	const machine_state_const_t *const current_state;

	//The new machine state. Write only authorised for position members;
	machine_state_p_t *const new_custom_state;

	//The distances array to store computed distances. Write is required;
	int16_t *const distances;

} distances_computation_args;


/*
 * A distances computer comprises some data, a function to compute new distances, and a destructor function;
 */

typedef struct {

	//The position target computer instance;
	void *const instance;

	//Compute the position for the new machine state;
	bool (*const compute)(const distances_computation_args *);

	//Delete the position computer. Required for cleanup;
	void (*const delete)(void *instance);

} distances_computer_t;


//------------------------------------------- controller builder computations -------------------------------------------

/*
 * Extra computations may be required, depending on the controller's complexity level.
 *
 * 	A controller computation is a function that takes current and new states (consts), makes calculations, and
 * 	saves them in the controller part of the movement builder;
 *
 * 	The controller part of the movement builder is not defined, and completely depends on the implementation;
 *
 * 	Computations can be disabled.
 */

/*
 * All computation functions take the following three arguments :
 */

typedef struct {

	//The movement builder to update;
	movement_builder_r_t *const builder_r;

	//The current state, read only;
	const machine_state_const_t *const current_state;

	//The new state, read only;
	const machine_state_const_t *const new_state;

} builder_computation_args;


typedef struct {

	//The status flag. Computation will be executed only if flag is set;
	bool enabled;

	//The function that will actually do the computation;
	void (*computation_function)(const builder_computation_args *args);

} controller_builder_computation_t;


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

/*
 * kinematic constraints all take the following three arguments
 */

typedef struct {

	//The time interval to store the constraint;
	time_interval_t *const duration_window;

	//The current state of the machine, const;
	const machine_state_const_t *const current_state;

	//The builder, constant;
	const movement_builder_const_t *const builder;

} duration_computation_args;


typedef struct {

	//The status flag. Constraint will be checked only if flag is set;
	bool enabled;

	//The function that will actually verify the constraint; Data will be stored in the provided interval;
	void (*constraint_function)(const duration_computation_args *args);

} kinematic_constraint_t;


//------------------------------------------- controller state updates -------------------------------------------

/*
 * Extra state fields may be required, depending on the controller's complexity level.
 *
 * 	A controller state computation is a function that takes current and new states (consts), makes calculations, and
 * 	saves them in the controller part of the movement builder;
 *
 * 	The controller part of the movement builder is not defined, and completely depends on the implementation;
 *
 * 	Computations can be disabled.
 */

/*
 * state computations take the following three arguments :
 */

typedef struct {

	//The current state read only;
	const machine_state_const_t *const current_state;

	//The movement builder, read only;
	const movement_builder_const_t *const const_builder;

	//The state, with write authorised to the controller part;
	const machine_state_l_t *const new_state;

} state_computation_args;


typedef struct {

	//The status flag. Computation will be executed only if flag is set;
	bool enabled;

	//The function that will actually do the computation;
	void (*computation_function)(const state_computation_args *args);

} controller_state_computation_t;


//------------------------------------------------- Machine Controller -------------------------------------------------

/*
 * A machine controller is composed of :
 * 	- a distances computer;
 * 	- an array of controller builder computations;
 * 	- an array of kinematic constraints
 * 	- an array of controller state computations;
 */

typedef struct {

	//---------------------------------- Dynamic structs management function pointers ----------------------------------

	//State creation function;
	void *(*controller_state_creator)();

	//State deletion function;
	void (*controller_state_deleter)(void *);

	//Builder creation function;
	void *(*controller_builder_creator)();

	//Builder deletion function;
	void (*controller_builder_deleter)(void *);


	//---------------------------------------------- Distance computation ----------------------------------------------

	//The number of different distance computers;
	const uint8_t nb_distances_computers;

	//The current distance computer;
	const uint8_t current_distance_computer;

	//Distance computers array;
	const distances_computer_t *const distances_computers;


	//----------------------------------------------- Builder computation ----------------------------------------------

	//The number of controller builder computations;
	const uint8_t nb_builder_computations;

	//The array of controller builder computations;
	const controller_builder_computation_t *const builder_computations;


	//---------------------------------------------- Kinematic constraints ---------------------------------------------

	//The number of kinematic constraints;
	const uint8_t nb_kinematic_constraints;

	//The array of kinematic constraints;
	const kinematic_constraint_t *const kinematic_constraints;


	//------------------------------------------------ State computation -----------------------------------------------

	//The number of controller state computations;
	const uint8_t nb_state_computations;

	//The array of controller state computations;
	const controller_state_computation_t *const state_computations;


} machine_controller_t;


//---------------------------------------------- Initialisation - deletion ---------------------------------------------

//Create and initialise a machine controller;
machine_controller_t *machine_controller_create(
	uint8_t nb_distances_computers, const distances_computer_t *distances_computer_c,
	uint8_t nb_builder_computations, const controller_builder_computation_t *builder_computations_c,
	uint8_t nb_kinematic_constraints, const kinematic_constraint_t *kinematic_constraints_c,
	uint8_t nb_state_computations, const controller_state_computation_t *state_computations_c,
	void *(*controller_builder_creator)(), void (*controller_builder_deleter)(void *),
	void *(*controller_state_creator)(), void (*controller_state_deleter)(void *)
);

//Delete a machine controller;
void machine_controller_delete(machine_controller_t *machine_controller);


//-------------------------------------------------- Structs creation --------------------------------------------------

//Create a controller state;
void *machine_controller_create_state(machine_controller_t *controller);

//Delete a controller state;
void machine_controller_delete_state(machine_controller_t *controller, void *state);


//Create a controller builder;
void *machine_controller_create_builder(machine_controller_t *controller);

//Delete a controller state;
void machine_controller_delete_builder(machine_controller_t *controller, void *builder);


//------------------------------------------------ Real time computation -----------------------------------------------


//Update the builder, by executing all builder computations;
void mcontroller_update_builder(const machine_controller_t *controller, const builder_computation_args *args);

//Determine the current movement's duration, evaluating all constraints successively;
void mcontroller_determine_duration(const machine_controller_t *controller, const duration_computation_args *args);

//Compute the controller's part of the state;
void mcontroller_compute_state(const machine_controller_t *controller, const state_computation_args *args);


#endif //TRACER_MACHINE_CONTROLLER_H
