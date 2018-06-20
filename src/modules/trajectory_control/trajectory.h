/*
  trajectory.h Part of TRACER

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


#ifndef TRACER_TRAJECTORY_H
#define TRACER_TRAJECTORY_H

#include "actuation.h"

#include "geometry.h"

#include "movement.h"

#include <data_structures/containers/circular_buffer.h>

/*
 * The trajectory controller's possible states;
 */
typedef enum {

		//The tracing routine is disabled;
		TCONTROLLER_STOPPED,

		//The tracing routine is enabled, the current movement is not finished;
		TCONTROLLER_STARTED,

		//The tracing routine is enabled, the current movement is finished but elementary movements are still to do.
		// No movement manipulation is authorised until all elementary movements are executed;
		TCONTROLLER_FINISHING,

} tcontroller_state_t;


typedef struct {

	//The trajectory's dimension;
	uint8_t dimension;

	//The trajectory controller's state;
	volatile tcontroller_state_t state;

	//The movements lists;
	linked_list_t movements;

	//The elementary movements buffer;
	cbuffer_t elementary_movements;

	//The trajectory's geometry;
	geometry_t *geometry;

	//The trajectory's actuation layer;
	actuation_t *actuation;

} tcontroller_t;


//--------------------------------------------------- Initialisation ---------------------------------------------------

//Create and initialise a trajectory controller;
tcontroller_t *tcontroller_create(uint8_t dimension, size_t elementary_buffer_size, geometry_t *geometry,
								  actuation_t *actuation);//TODO;

//Delete a trajectory controller;
void tcontroller_delete(tcontroller_t *);//TODO


//--------------------------------------------- Movement queue manipulation --------------------------------------------

//Enqueue a movement to the trajectory;
void tcontroller_enqueue(tcontroller_t *controller, movement_t *movement);//TODO;

//Remove movements from the end of the trajectory. Stops if all movements are removed;
void tcontroller_dequeue(tcontroller_t *controller, size_t nb_movements);//TODO


//---------------------------------------------------- Start - Stop ----------------------------------------------------

//Start moving;
void tcontroller_start(tcontroller_t *controller);//TODO;

//Stop moving. All undone movements / elementary movements will be deleted. Position is updated; Offset may appear;
void tcontroller_stop(tcontroller_t *controller);//TODO;

//TODO PAUSE PATCH:
//void tcontroller_set_pause_point(tcontroller_t *controller, bool (*restart_flag)());


//--------------------------------------- Functions called by the actuation layer --------------------------------------

//Get a new sub_movement to execute;
elementary_movement_t *trajectory_get_elementary_movement(tcontroller_t *);//TODO;

//Update the number of steps which will compose the current elementary movement;
void trajectory_update_movement_steps(tcontroller_t *, uint16_t mv_steps);//TODO;

//Execute a step in the processing of the next elementary movement;
void trajectory_sub_process(tcontroller_t *);//TODO;


#endif //TRACER_TRAJECTORY_H
