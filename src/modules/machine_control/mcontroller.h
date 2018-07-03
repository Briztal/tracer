//
// Created by root on 7/3/18.
//

#ifndef TRACER_MCONTROLLER_H
#define TRACER_MCONTROLLER_H

#include <stdint.h>

#include "machine.h"


/*
 * A machine state contains all data that describe the real time state of the machine;
 *
 * 	It comprises a mandatory part, that presents the actuation layer's current state, and a custom part,
 * 	that is left to the controller's implementation;
 */

typedef struct {

	//Actuation positions. Actuators commanded on an integer base;
	int32_t *actuation_positions;

	//Actuation speeds. Time is expressed in float, so are speeds;
	float *actuation_speeds;

	//The controller's state; Can contain whatever the controller decides;
	void *controller_state;

} machine_state_t;


//------------------------------------------- Controller builder computation -------------------------------------------

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

typedef struct {

	//Computations are linked;
	linked_element_t link;

	//The status flag. Computation will be executed only if flag is set;
	bool enabled;

	//The function that will actually do the computation;
	void (*computation_function)(const machine_state_t *current_state, const machine_state_t *new_state,
								 void *controller_builder);

} controller_computation_t;


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

typedef struct {

	//Constraints are linked;
	linked_element_t link;

	//The status flag. Constraint will be checked only if flag is set;
	bool enabled;

	//The function that will actually verify the constraint; Data will be stored in the provided interval;
	void (*constraint_function)(const machine_state_t *current_state, const void *controller_builder, interval_t *duration_window);

} kinematic_constraint_t;


//------------------------------------------------- Machine controller -------------------------------------------------


/*
 * A machine controller contain instances and methods pointer used to compute positions and speed targets;
 */

typedef struct {

	//---------------------------- Machine states ----------------------------

	//The current machine state;
	machine_state_t *machine_state;

	//The next machine state;
	machine_state_t *next_machine_state;

	//The machine state identifier; set when next_machine_state == s0;
	bool next_is_s0;

	//The first machine state;
	machine_state_t *const s0;

	//The second machine state;
	machine_state_t *const s1;


	//---------------------------- Geometry ----------------------------

	//The machine's geometry;
	const geometry_t *const geometry;


	//---------------------------- Position target computation ----------------------------

	//The position target computer instance;
	void *position_computer;

	//Compute the position for the new machine state; the (const) current state and new machine state are provided;
	void (*compute_position)(void *instance, const geometry_t *geometry,
							 const machine_state_t *current_state, machine_state_t *new_state);

	//Notify the position computer that the previously provided position has been accepted;
	void (*accept_position)(void *instance);

	//Notify the position computer that the previously provided position has been rejected;
	void (*reject_position)(void *instance);

	//Delete the position computer. Required for cleanup;
	void (*delete_position_computer)(void *instance);


	//---------------------------- Custom computations ----------------------------

	//List of extra computations;
	linked_list_t controller_computations;

	//Next computation to execute;
	controller_computation_t pending_computation;


	//---------------------------- kinematic constraints ----------------------------

	//The constraint manager;
	linked_list_t kinematic_constraints;

	//Next computation to execute;
	kinematic_constraint_t pending_constraints;


	//---------------------------- duration target computation ----------------------------

	//Duration target position computer;
	void *duration_computer;

	//Compute the target duration for the new movement; (const) current state and (const) movement builder are provided;
	void (*compute_movement_duration)(void *instance, const machine_state_t *current_state, const mbuilder_t *movement_builder);

	//Delete the speed computer. Required for cleanup;
	void (*delete_duration_computer)(void *instance);


} mcontroller_t;










#endif //TRACER_MCONTROLLER_H
