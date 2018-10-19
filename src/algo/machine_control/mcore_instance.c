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
 * cortex_m4f.mk must provide :
 * 	- NB_AXIS : the number of axis the machine core aims to control;
 */

//If one of the macro was not provided :

#if !defined(NB_AXIS) || !defined(NAME)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Define macros. Allows debugging on IDE environment;
#define NB_AXIS 5

#define NAME core_instance

#endif


//-------------------------------------------------- Headers --------------------------------------------------

#include "mcore.h"


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
	
	//Fixed dimension;
	.dimension = (uint8_t) NB_AXIS,
	
	//Variable fields not adjusted;
	.controller = 0,
	
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
	
	//Not initialised;
	.ready = false,
	
	//No movement receiver initialised;
	.movement = 0,
	
};
