/*
  machine_core.h Part of TRACER

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

#ifndef TRACER_MACHINE_CORE_H
#define TRACER_MACHINE_CORE_H

#include <stdint.h>

#include <stdbool.h>

#include <data_structures/containers/llist.h>


#include "machine.h"

#include "machine_states.h"

#include "machine_controller.h"


//------------------------------------------------- Machine controller -------------------------------------------------

/*
 * A machine controller contains the following :
 */

typedef struct {

	//---------------------------- Machine constants ----------------------------

	//The machine constant struct, owned, constant;
	const machine_constants_t machine_constants;


	//---------------------------- Machine states ----------------------------

	//Machine states, owned, mutable;
	machine_states_t states;


	//---------------------------- Movement builder ----------------------------

	//The movement builder, owned, mutable;
	movement_builder_t movement_builder;


	//---------------------------- Machine controller ----------------------------

	//The machine's controller, owned, mutable;
	machine_controller_t controller;


	//---------------------------- Actuation speed constraints ----------------------------

	//The actuation physics array, mutable;
	actuator_physics_t *const actuation_speed_constraints;


	//---------------------------- Movement queue ----------------------------
	//TODO


} machine_core_t;


#endif //TRACER_MACHINE_CORE_H
