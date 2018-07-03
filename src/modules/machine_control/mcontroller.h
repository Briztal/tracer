//
// Created by root on 7/3/18.
//

#ifndef TRACER_MCONTROLLER_H
#define TRACER_MCONTROLLER_H

#include <stdint.h>

#include "machine.h"


/*
 * A machine controller contain instances and methods pointer used to compute positions and speed targets;
 */

typedef struct {

	//---------------------------- Position target computation ----------------------------

	//The position target computer instance;
	void *position_computer;

	//Compute the position for the new machine state; the (const) current state and new machine state are provided;
	void (*compute_position)(void *instance, const machine_state_t *current_state, machine_state_t *new_state);

	//Notify the position computer that the previously provided position has been accepted;
	void (*accept_position)(void *instance);

	//Notify the position computer that the previously provided position has been rejected;
	void (*reject_position)(void *instance);


	//---------------------------- kinematic constraints ----------------------------

	//The constraint manager;
	void *constraints_manager;

	//Get the current number of kinematic constraints;
	uint8_t (*nb_kinematic_constraints)(void *instance);

	//Check a constraint. Next time the function is called, the next constraint will be checked;
	time_interval_t (*check_constraint)(void *instance);


	//TODO;
	//TODO;
	//TODO;
	//TODO;
	//TODO;
	//TODO;
	//TODO;
	//TODO;
	//TODO;
	//TODO;
	//TODO;

	//---------------------------- Speed target computation ----------------------------

	//Speed target position computer;
	void *speed_computer;

	//Compute the target speed for the new movement; (const) current state and (const) movement builder are provided;
	void (*compute_speed)(void *instance, const machine_state_t *current_state, const mbuilder_t *movement_builder);





};


#endif //TRACER_MCONTROLLER_H
