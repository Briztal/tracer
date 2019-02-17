//
// Created by root on 10/1/18.
//


//--------------------------------------------------- Headers --------------------------------------------------

#include <stdbool.h>

#include <kernel/exec/mod_hook>


#include <algo/machine_control/mcore_instance.h>

#include <algo/machine_control/mcontroller_instance.h>


//--------------------------------------------------- Dimension --------------------------------------------------

#define DIMENSION 2


//--------------------------------------------------- Actuators --------------------------------------------------



//--------------------------------------------------- Data --------------------------------------------------


//Declare our machine control structs; Make error if they are not defined;
DECLARE_MCORE(test_core);
DECLARE_MCONTROLLER(test_controller);

struct mmstate {
	
	//The machine state;
	struct mstate state;
	
	//TODO
};


int32_t act_0[DIMENSION];
int32_t act_1[DIMENSION];

float ctrl_0[DIMENSION];
float ctrl_1[DIMENSION];




struct mmstate state_0 = {
	.state = {
		.dimension = DIMENSION,
		.status = 0,
		.actuation_positions = act_0,
		.control_positions = ctrl_0,
	}
};


struct mmstate state_1 = {
	.state = {
		.dimension = DIMENSION,
		.status = 0,
		.actuation_positions = act_1,
		.control_positions = ctrl_1,
	}
};


struct cpt_data {

};

struct cpt_data data;





//--------------------------------------------------- Test --------------------------------------------------


bool mtest_init() {
	
	
	//Initialise the machine controller;
	mcon
	
	//Initialise the machine core;
	mcore_initialise(&test_core, &test_controller, (struct mstate *) &state_0, (struct mstate *) &state_1, &data);
	
	
	
	

}


KERNEL_HOOK_MODULE(USER_MODULE, mtest, &mtest_init);