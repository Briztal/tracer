//
// Created by root on 10/1/18.
//


//--------------------------------------------------- Headers --------------------------------------------------

#include <stdbool.h>

#include <mod/mod_hook>


//--------------------------------------------------- Test --------------------------------------------------

bool mb_speed_controller_init() {
	
	//Cache the speed controller;
	struct mcontroller *ctrl = &mb_speed_controller;
	
	
	/*
	 * Machine controller initialisation;
	 *
	 * 	The mb speed controller has only one axis, and doesn't require any extra computation or kinematic constraint;
	 */
	
	mcontroller_set_geometric_model(ctrl, (struct geometric_model *) &mb_geometry);
	
	mcontroller_set_distance_computer(ctrl, TODO);
	
	mcontroller_init_actuator_model(ctrl, 0, (struct actuator_model *) &mb_actuator);
	
	
	//Initialise the machine core;
	mcore_initialise(&mb_speed_core, &mb_speed_controller, &mb_state_0, &mb_state_1, &mb_cmp_data);
	
	
}
