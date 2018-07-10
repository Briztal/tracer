//
// Created by root on 7/10/18.
//

#ifndef TRACER_MACHINE_CONTROLLER_H
#define TRACER_MACHINE_CONTROLLER_H


#include <stdbool.h>
#include <modules/machine_control/OLDSHIT/mbuilder.h>

#include "machine.h"



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

	//Machine constants (dimension + geometry). Read only;
	const machine_constants_t *const machine;

	//The current machine state. Read only;
	const machine_state_const_t *const current_state;

	//The new machine state. Write only authorised for position members;
	machine_state_p_t *const new_custom_state;

	//The distances array to store computed distances. Write is required;
	int16_t *distances;

} distances_computation_args;


/*
 * A distances computer comprises some data, a function to compute new distances, and a destructor function;
 */

typedef struct {

	//The position target computer instance;
	void *instance;

	//Compute the position for the new machine state;
	bool (*compute)(void *instance, distances_computation_args);

	//Delete the position computer. Required for cleanup;
	void (*delete)(void *instance);

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
	void (*computation_function)(builder_computation_args *args);

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
	void (*constraint_function)(duration_computation_args *args);

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
	const machine_state_const_t *current_state;

	//The movement builder, read only;
	const movement_builder_const_t *const_builder;

	//The state, with write authorised to the controller part;
	const machine_state_l_t *new_state;

} state_computation_args;

typedef struct {

	//The status flag. Computation will be executed only if flag is set;
	bool enabled;

	//The function that will actually do the computation;
	void (*computation_function)(state_computation_args *args);

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

	//The distance computer; Stored by pointer so that it can be modified;
	distances_computer_t *distances_computer;

	//The array of controller builder computations;
	controller_builder_computation_t *builder_computations;

	//The array of controller builder computations;
	kinematic_constraint_t *kinematic_constraints;

	//The array of controller builder computations;
	controller_state_computation_t *state_computations;

} machine_controller_t;


//Compute distances;
void mcontroller_compute_distances(machine_controller_t *controller, distances_computation_args *args);

//Update the builder, by executing all builder computations;
void mcontroller_update_builder(machine_controller_t *controller, builder_computation_args *args);

//Determine the current movement's duration, evaluating all constraints successively;
void mcontroller_determine_duration(machine_controller_t *controller, duration_computation_args *args);

//Compute the controller's part of the state;
void mcontroller_compute_state(machine_controller_t *controller, state_computation_args *args);



#endif //TRACER_MACHINE_CONTROLLER_H
