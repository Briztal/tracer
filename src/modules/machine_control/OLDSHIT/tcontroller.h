/*
  tcontroller.h Part of TRACER

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


#ifndef TRACER_TCONTROLLER_H
#define TRACER_TCONTROLLER_H

#include <data_structures/containers/llist.h>

#include <data_structures/containers/circular_buffer.h>


#include "movement.h"

#include "mbuilder.h"

#include "trajectory.h"


//------------------------------------------------- Trajectory control -------------------------------------------------

/*
 * The trajectory controller's possible states;
 */
typedef enum {

	//The tracing routine is disabled;
		TCONTROLLER_STOPPED,

	//The tracing routine is enabled, the current movement is not finished;
		TCONTROLLER_STARTED,

	//The tracing routine is enabled, the current movement is finished but elementary trajectories are still to do.
	// No movement manipulation is authorised until all elementary trajectories are executed;
		TCONTROLLER_FINISHING,

} tcontroller_state_t;


typedef struct {

	/*
	 * Components;
	 */

	//The trajectory's dimension;
	const uint8_t dimension;


	//The trajectories lists;
	linked_list_t trajectories;

	//The movement builder;
	mbuilder_t movement_builder;

	//The movements buffer;
	cbuffer_t movements;


	/*
	 * Optimisations;
	 */

	//Acceleration monitoring enabled;
	bool acceleration_monitoring_enabled;

	//Jerk monitoring enabled;
	bool jerk_monitoring_enabled;


	/*
	 * Real time data;
	 */

	//The trajectory controller's state;
	volatile tcontroller_state_t state;



} tcontroller_t;



typedef struct {

	/*
	 * Components;
	 */

	//The number of actuator;
	const uint8_t dimension;

	//The geometry pointer;
	geometry_t *const geometry;


	/*
	 * Config;
	 */

	//The maximal number of buffered movements:
	const size_t movement_buffer_size;

	//The target movement distance;
	uint16_t resolution;


	/*
	 * Optimisations;
	 */

	//Acceleration monitoring enabled;
	bool acceleration_monitoring_enabled;

	//Jerk monitoring enabled;
	bool jerk_monitoring_enabled;


} tcontroller_config_t;


//--------------------------------------------------- Initialisation ---------------------------------------------------

//Create and initialise a trajectory controller;
tcontroller_t *tcontroller_create(tcontroller_config_t *config);

//Delete a trajectory controller;
void tcontroller_delete(tcontroller_t *);


//--------------------------------------------- Movement queue manipulation --------------------------------------------

//Enqueue a movement to the trajectory;
void tcontroller_enqueue(tcontroller_t *tcontroller, trajectory_t *movement);

//Remove trajectories from the end of the trajectory. Stops if all trajectories are removed;
//void tcontroller_dequeue(tcontroller_t *tcontroller, size_t nb_movements);


//---------------------------------------------------- Start - Stop ----------------------------------------------------

//Start moving;
void tcontroller_init(tcontroller_t *tcontroller);

//Stop moving. All undone trajectories / elementary trajectories will be deleted. Position is updated; Offset may appear;
void tcontroller_exit(tcontroller_t *tcontroller);

//TODO PAUSE PATCH:
//void tcontroller_set_pause_point(tcontroller_t *controller, bool (*restart_flag)());


//--------------------------------------- Functions called by the actuation layer --------------------------------------

//Get a new sub_movement to execute;
movement_t *tcontroller_get_movement(tcontroller_t *);

//Update the number of steps which will compose the current elementary movement;
void tcontroller_discard_movement(tcontroller_t *, uint16_t mv_steps);//TODO;

//Execute a step in the processing of the next elementary movement;
void tcontroller_step_process(tcontroller_t *);//TODO;


#endif //TRACER_TCONTROLLER_H
