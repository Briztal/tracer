/*
  mcontroller.h Part of TRACER

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

#ifndef TRACER_MCONTROLLER_H
#define TRACER_MCONTROLLER_H

#include <stdint.h>

#include <stdbool.h>

#include <data_structures/containers/llist.h>

#include "machine_controller.h"
#include "machine_core.h"
#include "machine_states.h"




//------------------------------------------------- Machine controller -------------------------------------------------

/*
 * A machine controller contain instances and methods pointer used to compute positions and speed targets;
 */

typedef struct {

	//---------------------------- Machine constants ----------------------------

	machine_constants_t machine_constants;


	//---------------------------- Machine states ----------------------------

	machine_states_t states;


	//---------------------------- Movement builder ----------------------------

	movement_buinlder_t movement_builder;


	//---------------------------- Machine controller ----------------------------

	//The machine's controller;
	machine_controller_t controller;


	//---------------------------- Actuation speed constraints ----------------------------

	//The actuation physics array;
	actuation_speed_constraint_t *actuation_speed_constraints;


	//TODO---------------------------- Movement queue ----------------------------


} machine_core_t;


#endif //TRACER_MCONTROLLER_H
