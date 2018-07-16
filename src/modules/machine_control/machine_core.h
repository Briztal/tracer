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

#include <data_structures/containers/non_concurrent/llist.h>
#include "../../../TO_REINTEGRATE/computation_network.h"


#include "machine.h"

#include "machine_states.h"

#include "machine_controller.h"

#include "movement_builder.h"


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
	machine_controller_t *const controller;


	//---------------------------- Actuation speed constraints ----------------------------

	//The actuation physics array, mutable;
	const actuator_physics_t *const actuation_physics;


	//---------------------------- Movement buffer ----------------------------

	//The movement buffer, owned, mutable;
	cbuffer_t movement_buffer;


	//---------------------------- Movement computation ----------------------------

	//The movement processor;
	cnetwork_t *movement_computation;


} machine_core_t;


//Create a machine core from machine constants, a controller, and an array of actuator physics managers;
machine_core_t *machine_core_create(const machine_constants_t *machine_constants,
									machine_controller_t *controller, uint8_t nb_actuator_physics,
									const actuator_physics_t *actuator_physics, size_t movement_buffer_size);

//Execute some stages in the movement computation;
void machine_core_compute_movements(machine_core_t *core, uint8_t nb_stages);

//Get the number of available movements;
size_t machine_core_available_movements(machine_core_t *core);

//Read the movement to execute;
movement_t *read_movement(machine_core_t *core);

//Discard the first movement to execute;
void discard_movement(machine_core_t *core);

//Delete a previously created machine controller;
void machine_core_delete(machine_core_t *machine_core);



#endif //TRACER_MACHINE_CORE_H
