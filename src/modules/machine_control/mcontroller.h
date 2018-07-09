//
// Created by root on 7/3/18.
//

#ifndef TRACER_MCONTROLLER_H
#define TRACER_MCONTROLLER_H

#include <stdint.h>

#include <stdbool.h>

#include <data_structures/containers/llist.h>

#include "machine.h"

#include "movement.h"
#include "distances_computer.h"
#include "machine_states.h"




//------------------------------------------- Movement builder -------------------------------------------

/*
 * Movements are computed step by step, and temporary data is stored in the movement builder.
 */

typedef struct {

	//The movement that we currently compute; References the distances array and the duration;
	movement_t *movement;

	//The time interval array, containing the duration window for each actuator during the computation;
	time_interval_t *interval_array;

	//The distance re-computation queue;
	ts_queue recomputation_queue;

	//The duration interval that satisfies all constraints;
	time_interval_t duration_interval;

	//After distances were provided, they might be modified to avoid breaking actuation limitations. This flag is
	//set when they are modified;
	bool distances_altered;

	//The controller builder. Can contain whatever the controller decides;
	void *controller_builder;

} movement_builder_t;


//------------------------------------------- Actuator physical limitation -------------------------------------------

/*
 * Actuators have physical limitations, that we must absolutely respect, otherwise, actuators or the machine could
 * 	be damaged;
 *
 * 	This structure contains a function and an eventual instance, that will determine the duration limit interval;
 */

typedef struct {

	//The instance to pass when the duration interval is computed;
	void *instance;

	//The function to compute the time interval;
	void (*compute_duration_interval)(void *instance, time_interval_t interval);

	//The function to compute the minimal distance to fit duration requirements;
	void (*compute_minimal_distance)(void *instance, uint16_t *maximal_distance, float movement_duration);

	//The adjustment flag. If set, distance can be adjusted to comply with the physical constraint;
	bool distance_adjustable;

	time_interval_t *time_interval;

} actuation_speed_constraint_t;


//------------------------------------------- Distance re-computation  -------------------------------------------

/*
 * When re-computing a distance, the constraint and the distance must be known;
 */

typedef struct {

	//The pointer to the actuation constraint the distance must conform to;
	actuation_speed_constraint_t *constraint;

	//The location to store the final distance;
	uint16_t *final_distance_p;

};


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
	void (*constraint_function)(const machine_state_t *current_state, const void *controller_builder,
								time_interval_t *duration_window);

} kinematic_constraint_t;


//------------------------------------------------- Machine controller -------------------------------------------------

/*
 * A machine controller contain instances and methods pointer used to compute positions and speed targets;
 */

typedef struct {

	//---------------------------- Machine constants ----------------------------

	machine_constants_t machine_constants;


	//---------------------------- Machine states ----------------------------

	machine_states_t states;


	//---------------------------- Movement builder ----------------------------

	movement_builder_t movement_builder;


	//---------------------------- Target distances computation ----------------------------

	distances_computer_t distances_computer;


	//---------------------------- Actuation speed constraints ----------------------------

	actuation_speed_constraint_t *actuation_speed_constraints;


	//---------------------------- Custom computations ----------------------------

	//List of extra computations;
	linked_list_t controller_computations;

	//Next computation to execute;
	controller_computation_t *pending_computation;


	//---------------------------- kinematic constraints ----------------------------

	//The constraint manager;
	linked_list_t kinematic_constraints;

	//Next computation to execute;
	kinematic_constraint_t *pending_constraint;


	//---------------------------- duration target computation ----------------------------

	//Duration target position computer;
	void *duration_computer;

	//Compute the target duration for the new movement; (const) current state and (const) movement builder are provided;
	void (*compute_movement_duration)(void *instance, const machine_state_t *current_state,
									  const movement_builder_t *movement_builder);

	//Delete the speed computer. Required for cleanup;
	void (*delete_duration_computer)(void *instance);


	//TODO---------------------------- Movement queue ----------------------------


} mcontroller_t;


#endif //TRACER_MCONTROLLER_H
