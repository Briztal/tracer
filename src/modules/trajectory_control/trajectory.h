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

#include "trajectory_t.h"

#include "actuation.h"

#include "movement_t.h"



//--------------------------------------- Functions called by the actuation layer --------------------------------------

//Get a new sub_movement to execute;
elementary_movement_t *trajectory_get_elementary_movement(trajectory_t *);

//Update the number of steps which will compose the current elementary movement;
void trajectory_update_movement_steps(trajectory_t *, uint16_t mv_steps);

//Execute a step in the processing of the next elementary movement;
void trajectory_sub_process(trajectory_t *);


#endif //TRACER_TRAJECTORY_H
