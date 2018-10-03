/*
  mcontroller_instance.h Part of TRACER

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
 * Compiling this file and linking it in your project will generate an entirely functional machine controller instance;
 *
 * It takes several macro parameters, that are defined below;
 *
 * All symbols except the machine controller struct are static.
 */


//--------------------------------------------------- Make parameters --------------------------------------------------

/*
 * Makefile must provide :
 * 	- NB_AXIS : the number of axis the machine core aims to control;
 */

//If one of the macro was not provided :

#if !defined(NB_AXIS) || !defined(NB_BUILDER_CPTS) || !defined(NB_KINEMATIC_CSTRS) || !defined(NB_STATE_CPTS) || !defined(NAME)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Define macros. Allows debugging on IDE environment;
#define NB_AXIS 5

#define NB_BUILDER_CPTS 5

#define NB_KINEMATIC_CSTRS 5

#define NB_STATE_CPTS 5

#define NAME core_instance

#endif


//------------------------------------------------------- Headers ------------------------------------------------------

#include "mcontroller.h"

#include "actuator_model.h"


//--------------------------------------------------- Actuators model --------------------------------------------------

//Declare the array of actuators; Initialised to 0 for instance;
struct actuator_model actuators[NB_AXIS] = {0};


//------------------------------------------------- Builder computation ------------------------------------------------

//Declare the array of builder computation function pointers; Initialised to 0 for instance;
void *builder_cmp[NB_BUILDER_CPTS] = {0};


//------------------------------------------------ Kinematic constraints -----------------------------------------------

//Declare the array of builder computation function pointers; Initialised to 0 for instance;
void *kinematic_cnst[NB_KINEMATIC_CSTRS] = {0};


//-------------------------------------------------- State computation -------------------------------------------------

//Declare the array of builder computation function pointers; Initialised to 0 for instance;
void *state_cmp[NB_STATE_CPTS] = {0};


//------------------------------------------------- Machine controller -------------------------------------------------

/*
 * A machine controller is composed of :
 * 	- a distances dist_computer;
 * 	- an array of controller builder computations;
 * 	- an array of kinematic constraints
 * 	- an array of controller state computations;
 */

static struct mcontroller controller = {
	
	//Dimension provided;
	.dimension = NB_AXIS,
	
	//Geometric model not provided for instance;
	.geometry = 0,
	
	//Distance dist_computer not provided for instance;
	.compute_distances = 0,
	
	
	//Provide access to the actuators array;
	.actuators_models = actuators,
	
	
	//Number of builder computations provided;
	.nb_builder_cpts = NB_BUILDER_CPTS,
	
	//Provide access to the builder computations array;
	.builder_cpts = (void (**)(const struct mstate *const, const struct mstate *const, void *const)) builder_cmp,
	
	
	//Number of kinematic constraints provided;
	.nb_kinematic_cnsts = NB_KINEMATIC_CSTRS,
	
	//Provide access to the kinematic constraints array;
	.kinematic_cnsts = (void (**)(const struct mstate *, const void *, struct time_interval *)) kinematic_cnst,
	
	
	//Number of state computations provided;
	.nb_state_cpts = NB_STATE_CPTS,
	
	//Provide access to the state computations array;
	.state_cpts = (void (**)(const struct mstate *const, const struct computation_data *const,
							 struct mstate *const)) state_cmp,
	
	
	//All data is uninitialised;
	.nb_uninitialised_actuators = NB_AXIS,
	.nb_uninitialised_builder_cpts = NB_BUILDER_CPTS,
	.nb_uninitialised_kinematic_cnsts = NB_KINEMATIC_CSTRS,
	.nb_uninitialised_state_cpts = NB_STATE_CPTS,
	
	//Not ready for instance;
	.ready = false,
	
};

