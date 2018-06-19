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


typedef struct {

	//The trajectory's dimension;
	uint8_t dimension;

	//The movements lists;
	linked_list_t movements;

	//The trajectory's geometry;
	geometry_t *geometry;

	//The trajectory's actuation layer;
	actuation_t *actuation;

} trajectory_controller_t;

//--------------------------------------------------- Initialisation ---------------------------------------------------

//Create a trajectory;
trajectory_controller_t *trajectory_init();//TODO;

//Delete a trajectory;
void trajectory_delete(trajectory_controller_t *);//TODO


//------------------------------------------------ Movement manipulation -----------------------------------------------

//Add a movement to the trajectory;
void trajectory_enqueue_movement(movement_t *movement);//TODO;

//Delete a trajectory;
void trajectory_delete(trajectory_controller_t *);//TODO

//TODO START STOP

//--------------------------------------- Functions called by the actuation layer --------------------------------------

//Get a new sub_movement to execute;
elementary_movement_t *trajectory_get_elementary_movement(trajectory_controller_t *);

//Update the number of steps which will compose the current elementary movement;
void trajectory_update_movement_steps(trajectory_controller_t *, uint16_t mv_steps);

//Execute a step in the processing of the next elementary movement;
void trajectory_sub_process(trajectory_controller_t *);


#endif //TRACER_TRAJECTORY_H
