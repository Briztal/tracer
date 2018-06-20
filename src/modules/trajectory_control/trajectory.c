/*
  trajectory.c Part of TRACER

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

#include <kernel/kernel.h>
#include <data_structures/containers/llist.h>
#include "trajectory.h"

//--------------------------------------------------- Private headers --------------------------------------------------

//Wait till the state allows movement manipulation;
void tcontroller_state_wait();

//Deactivate temporarily the actuation interrupt;
inline void tcontroller_deactivate_actuation(const tcontroller_t *controller);

//Reactivate the actuation interrupt if necessary;
inline void tcontroller_reactivate_actuation(const tcontroller_t *controller);


//------------------------------------------------------- Inline -------------------------------------------------------

//Deactivate temporarily the actuation interrupt;
inline void tcontroller_deactivate_actuation(const tcontroller_t *const controller) {

	//If the actuation is started :
	if (controller->state != TCONTROLLER_STOPPED) {

		//Pause the actuation layer;
		actuation_pause(controller->actuation);

	}

}

//Reactivate the actuation interrupt if necessary;
inline void tcontroller_reactivate_actuation(const tcontroller_t *const controller) {

	//If the actuation is started :
	if (controller->state != TCONTROLLER_STOPPED) {

		//Resume the actuation layer;
		actuation_resume(controller->actuation);

	}

}


/*
 * This function will return after making sure that the movement queue can be safely manipulated.
 */

void tcontroller_enter_queue_critical_section(const tcontroller_t *const controller) {

	//Deactivate temporarily the actuation layer if necessary;
	tcontroller_deactivate_actuation(controller);

	//If the controller is finishing its trajectory :
	if (controller->state == TCONTROLLER_FINISHING) {

		//Reactivate the actuation layer;
		tcontroller_reactivate_actuation(controller);

		//Wait till finishing state has changed;
		while (controller->state == TCONTROLLER_FINISHING);

		//Deactivate temporarily the actuation layer;
		tcontroller_deactivate_actuation(controller);

	}

}


/*
 * This function will return after making sure that the movement queue can be safely manipulated.
 */

inline void tcontroller_leave_queue_critical_section(const tcontroller_t *const controller) {

	//Reactivate the actuation layer;
	tcontroller_reactivate_actuation(controller);

}


//--------------------------------------------------- Initialisation ---------------------------------------------------

/*
 * tcontroller_create : creates and initialises a trajectory controller;
 *
 * 	Dimension checks are made in all components, a kernel error is raised if they don't match;
 */

tcontroller_t *tcontroller_create(uint8_t dimension, size_t elementary_buffer_size, geometry_t *geometry,
								  actuation_t *actuation) {

	//Create the initializer;
	tcontroller_t init = {

		//Copy the dimension;
		.dimension = dimension,

		//Initialise the linked list;
		.movements = EMPTY_LINKED_LIST(255),

		//Initialise the cbuffer. Will be resized to its max size right after:
		.elementary_movements = EMPTY_CBUFFER(elementary_movement_t, elementary_buffer_size),

		//Save the geometry pointer;
		.geometry = geometry,

		//Save the actuation pointer;
		.actuation = actuation,

	};

	//Resize the elementary buffer to its maximal size;
	cbuffer_resize(&init.elementary_movements, elementary_buffer_size);

	//Allocate heap memory, initialise it and return its pointer;
	return kernel_malloc_copy(sizeof(tcontroller_t), &init);

}


//Delete a trajectory controller;
void tcontroller_delete(tcontroller_t *tcontroller) {

	//TODO STOP

	//TODO DELETE REMAINING ITEMS;

}


//--------------------------------------------- Movement queue manipulation --------------------------------------------

/*
 * tcontroller_enqueue :
 * 	- waits for the movement queue to be safely manipulable;
 * 	- enqueue a movement to the trajectory;
 */

void tcontroller_enqueue(tcontroller_t *const controller, movement_t *const movement) {

	//Enter in a queue critical section;
	tcontroller_enter_queue_critical_section(controller);

	//Add the movement at the end of the controller's list;
	llist_insert_last(&controller->movements, (linked_element_t *) movement);

	//Leave the queue critical section;
	tcontroller_leave_queue_critical_section(controller);

}

/*
 * tcontroller_dequeue : removes movements from the end of the trajectory. Stops if all movements are removed;
 */

void tcontroller_dequeue(tcontroller_t *controller, size_t nb_movements) {

	//Enter in a queue critical section;
	tcontroller_enter_queue_critical_section(controller);

	//If we must remove more movements that the controller has :
	if (nb_movements >= controller->movements.nb_elements) {

		//Stop the trajectory controller, purge all movements and elementary movements;
		tcontroller_stop(controller);

	} else {

		//For each time a movement must be removed
		while (nb_movements--) {

			//Cache and remove the last movement of the queue;
			movement_t *movement = (movement_t *) llist_remove_first(&controller->movements);

			//Delete the movement;
			movement_delete(movement);

		}

	}

	//Leave the queue critical section;
	tcontroller_leave_queue_critical_section(controller);

}


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