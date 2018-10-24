/*
  mcore_instance.h Part of TRACER

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

/*
 * Compiling this file and linking it in your project will generate an entirely functional machine core instance;
 *
 * It takes several macro parameters, that are defined below;
 *
 * All symbols except the machine core struct are static.
 */


//--------------------------------------------------- Make parameters --------------------------------------------------

/*
 * Makefile must provide :
 * 	- INSTANCE_NAME : the name of the machine controller instance;
 * 	- NB_AXIS : the number of axis the machine core aims to control;
 * 	- NB_BUILDER_CPTS : the number of builder computation stages;
 * 	- NB_KINEMATIC_CSTRS : the number of builder computation stages;
 * 	- NB_STATE_CPTS : the number of builder computation stages;
 */

#if !defined(INSTANCE_NAME)

//Log
#error "Name axis not provided. Check the Makefile;"

//Define macros. Allows debugging on IDE environment;
#define INSTANCE_NAME instance

#endif


#if !defined(NB_AXIS)

//Log
#error "Number of axis not provided. Check the Makefile;"

//Define macros. Allows debugging on IDE environment;
#define NB_AXIS 5

#endif


#if !defined(NB_BUILDER_CPTS)

//Log
#error "Number of builder computations not provided. Check the Makefile;"

//Define macros. Allows debugging on IDE environment;
#define NB_BUILDER_CPTS 5

#endif


#if !defined(NB_KINEMATIC_CSTRS)

//Log
#error "Number of kinematic constraints not provided. Check the Makefile;"

//Define macros. Allows debugging on IDE environment;
#define NB_KINEMATIC_CSTRS 5

#endif


#if !defined(NB_STATE_CPTS)

//Log
#error "Number of state computations not provided. Check the Makefile;"

//Define macros. Allows debugging on IDE environment;
#define NB_STATE_CPTS 5

#endif


//------------------------------------------------------ Includes ------------------------------------------------------

#include <macro/incr_call.h>
#include "mcore.h"


//------------------------------------------------------ Helpers ------------------------------------------------------

#define _NCT(a, b) a##b
#define NCT(a, b) _NCT(a, b)

//A macro to generate a proper variable name;
#define NM(var) NCT(var, NSTANCE_NAME)


#define _NCTI(a, b, i) a##_##b##_##i
#define NCTI(a, b, i) _NCTI(a, b, i)

//A macro to generate a proper variable name;
#define NMI(var, i) NCTI(var, i, INSTANCE_NAME)


//---------------------------------------------------- Computation Model -----------------------------------------------------


//The machine's geometric_model, mutable ref;
extern struct geometric_model geometry;


//Declare a builder computation;
#define DECLARE_ACT_MDL(i) extern struct actuator_model NMI(act, i);

//Declare all builders computations;
INCR_CALL(NB_BUILDER_CPTS, DECLARE_ACT_MDL)

//Initialise a builder computation in an array;
#define REGISTER_ACT_MDL(i) &NMI(act, i),

//The actuation physics array, mutable ref;
static struct actuator_model *actuators_models[NB_AXIS] = { //TODO PRE_INITIALISE

INCR_CALL(NB_BUILDER_CPTS, REGISTER_ACT_MDL)

};

#undef REGISTER_ACT_MDL

#undef REGISTER_ACT_MDL


//Declare a builder computation;
#define DECLARE_BUILD_CMP(i)\
void NMI(bldr, i) (const struct mstate * current_state, const struct mstate * new_state, void * const_builder);

//Declare all builders computations;
INCR_CALL(NB_BUILDER_CPTS, DECLARE_BUILD_CMP)

//Initialise a builder computation in an array;
#define REGISTER_BUILDER(i) &NMI(bldr, i),

//Create the builder computations array;
static builder_cpt builder_computations[NB_BUILDER_CPTS] = {
	
	INCR_CALL(NB_BUILDER_CPTS, REGISTER_BUILDER)
	
};

#undef DECLARE_BUILD_CMP

#undef REGISTER_BUILDER



//Declare a kinematic constraint;
#define DECLARE_CONSTRAINT(i) void NMI(cstrt, i) (const struct mstate *current_state, const void *controller_data, struct time_interval *duration_window);

//Declare all kinematic constraints;
INCR_CALL(NB_KINEMATIC_CSTRS, DECLARE_CONSTRAINT)

//Initialise a builder computation in an array;
#define REGISTER_CNST(i) &NMI(cstrt, i),

//Create the constraints array;
static kinematic_cnst kinematic_constraints[NB_KINEMATIC_CSTRS] = {
	
	INCR_CALL(NB_KINEMATIC_CSTRS, REGISTER_CNST)
	
};

#undef DECLARE_CONSTRAINT

#undef REGISTER_CNST



//Declare a state computation;
#define DECLARE_STATE_CMP(i)\
void NMI(state, i)(const struct mstate * current_state, const struct computation_data * , struct mstate * new_state);

//Declare all state computations;
INCR_CALL(NB_KINEMATIC_CSTRS, DECLARE_STATE_CMP)

//Initialise a builder computation in an array;
#define REGISTER_STATE(i) &NMI(state, i),

//Create the state computations array;
static state_cpt state_computations[NB_STATE_CPTS] = {
	
	INCR_CALL(NB_STATE_CPTS, REGISTER_STATE)
	
};

#undef DECLARE_STATE_CMP

#undef REGISTER_STATE



//-------------------------------------------------- Computation vars --------------------------------------------------

//The array of time intervals;
static struct time_interval intervals[NB_AXIS];

//The time interval to merge the previous array in;
static struct time_interval final_interval;


//---------------------------------------------------- Machine core ----------------------------------------------------

/*
 * Define the movement core and transmit access to all previously defined data structures;
 */

static struct mcore core = {
	
	//Save sizes;
	.nb_axis = (uint8_t) NB_AXIS,
	.nb_builder_cpts = (uint8_t) NB_BUILDER_CPTS,
	.nb_kinematic_cnsts = (uint8_t) NB_KINEMATIC_CSTRS,
	.nb_state_cpts = (uint8_t) NB_STATE_CPTS,
	
	
	//Distances computer not initialised for instance;
	.dist_computer = {0},
	
	//All other model structs initialised;
	.geometry = &geometry,
	.actuators_models = actuators_models, //TODO NOP !!!
	.builder_cpts = builder_computations,
	.kinematic_cnsts = kinematic_constraints,
	.state_cpts = state_computations,
	
	
	//Computation states defined;
	.states = {
		
		//Candidate state is s1;
		.current_is_s0 = false,
		
		//Current state is s0;
		.current_state = 0,
		
		//Candidate state is s1;
		.next_state = 0,
		
		//s0 init. Transmit arrays;
		.s0 = 0,
		
		//s1 init. Transmit arrays;
		.s1 = 0,
		
	},
	
	//Movement builder;
	.cmp_data = {
		
		//Provide access to the array;
		.interval_array = intervals,
		
		//Initialise the duration interval to the largest width:
		.final_interval = &final_interval,
		
		//No controller is registered for instance;
		.controller_data = 0,
		
	},
	
	//No movement receiver initialised;
	.movement = 0,
	
};
