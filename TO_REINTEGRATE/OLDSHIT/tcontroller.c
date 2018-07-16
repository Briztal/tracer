/*
  tcontroller.c Part of TRACER

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
#include "tcontroller.h"

#include <kernel/kernel.h>

//--------------------------------------------------- Private headers --------------------------------------------------

//Wait till the state allows movement manipulation;
void tcontroller_state_wait();

//Deactivate temporarily the actuation interrupt;
inline void tcontroller_deactivate_actuation(const tcontroller_t *controller);

//Reactivate the actuation interrupt if necessary;
inline void tcontroller_reactivate_actuation(const tcontroller_t *controller);


//------------------------------------------------------- Inline -------------------------------------------------------

//Deactivate temporarily the actuation interrupt;
/*
inline void tcontroller_deactivate_actuation(const tcontroller_t *const controller) {


	//If the actuation is started :
	if (controller->state != TCONTROLLER_STOPPED) {

		//Pause the actuation layer;
		actuation_pause(controller->actuation);

	}

}
 */


//Reactivate the actuation interrupt if necessary;
/*inline void tcontroller_reactivate_actuation(const tcontroller_t *const controller) {

	//If the actuation is started :
	if (controller->state != TCONTROLLER_STOPPED) {

		//Resume the actuation layer;
		actuation_resume(controller->actuation);

	}

}
 */


/*
 * This function will return after making sure that the movement queue can be safely manipulated.
 */
/*
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

}*/


/*
 * This function will return after making sure that the movement queue can be safely manipulated.
 */
/*
inline void tcontroller_leave_queue_critical_section(const tcontroller_t *const controller) {

	//Reactivate the actuation layer;
	tcontroller_reactivate_actuation(controller);

}*/


//--------------------------------------------------- Initialisation ---------------------------------------------------

/*
 * tcontroller_create : creates and initialises a trajectory controller;
 *
 * 	Dimension checks are made in all components, a kernel error is raised if they don't match;
 */

tcontroller_t *tcontroller_create(const uint8_t dimension, const size_t movements_buffer_size, geometry_t *const geometry) {

	//Create the initializer;
	tcontroller_t init = {

		//Copy the dimension;
		.dimension = dimension,

		//Initialise the linked list;
		.trajectories = EMPTY_LINKED_LIST(255),

		//Initialise the cbuffer. Will be resized to its max size right after:
		.movements = EMPTY_CBUFFER(movement_t, movements_buffer_size),

		//Save the geometry pointer;
		.geometry = geometry,

	};

	//Resize the movements buffer to its maximal size;
	cbuffer_resize(&init.movements, movements_buffer_size);

	//TODO INITIALISE POSITIONS;

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

void tcontroller_enqueue(tcontroller_t *const tcontroller, trajectory_t *const movement) {

	//Enter in a queue critical section;
	//TODO tcontroller_enter_queue_critical_section(tcontroller);

	//Add the movement at the end of the controller's list;
	llist_insert_last(&tcontroller->trajectories, (linked_element_t *) movement);

	//Leave the queue critical section;
	//TODO tcontroller_leave_queue_critical_section(tcontroller);

}

/*
 * tcontroller_dequeue : removes trajectories from the end of the trajectory. Stops if all trajectories are removed;
 */

void tcontroller_dequeue(tcontroller_t *const tcontroller, size_t nb_movements) {

	//Enter in a queue critical section;
	//TODO tcontroller_enter_queue_critical_section(tcontroller);

	//If we must remove more trajectories that the controller has :
	if (nb_movements >= tcontroller->trajectories.nb_elements) {

		//Stop the trajectory controller, purge all trajectories and movements;
		tcontroller_exit(tcontroller);

	} else {

		//For each time a movement must be removed
		while (nb_movements--) {

			//Cache and remove the last movement of the queue;
			trajectory_t *movement = (trajectory_t *) llist_remove_first(&tcontroller->trajectories);

			//Delete the trajectory;
			trajectory_delete(movement);

		}

	}

	//Leave the queue critical section;
	//TODO tcontroller_leave_queue_critical_section(tcontroller);

}


//---------------------------------------------------- Start - Stop ----------------------------------------------------

/*
 * tcontroller_init : starts the movement procedure, if :
 * 	- it is stopped;
 * 	- trajectories are present in the queue, or if not, movements are to be processed;
 */

void tcontroller_init(tcontroller_t *const tcontroller) {

	//If the controller is not stopped :
	if (tcontroller->state != TCONTROLLER_STOPPED) {

		//Do nothing;
		return;

	}

	//Cache the new state;
	tcontroller_state_t state;

	//If the controller has trajectories available :
	if (tcontroller->trajectories.nb_elements) {

		//The controller will be in the started state;
		state = TCONTROLLER_STARTED;

	} else if (tcontroller->movements.nb_elements) {

		//If the controller has movements available, it will be in the finishing state;
		state = TCONTROLLER_FINISHING;

	} else {

		//If no trajectories / movements are available, do nothing;
		return;

	}

	//TODO INITIALISE GEOMETRY;
	//TODO INITIALISE PHYSICS LAYER

	//Update the state;
	tcontroller->state = (volatile tcontroller_state_t) state;

	//Start the actuation layer, providing our instance. Movement will be queried immediately;
	//TODO actuation_start(tcontroller->actuation, tcontroller);

}


//Stop moving. All undone trajectories / movements will be terminated. Position is updated; Offset may appear;
void tcontroller_exit(tcontroller_t *const tcontroller) {

	//Stop the actuation layer;
	//TODO actuation_stop(tcontroller->actuation);

	//Cache the movement linked list;
	linked_list_t *trajectories = &tcontroller->trajectories;

	//Delete all trajectories;
	for (size_t movement_index = trajectories->nb_elements; movement_index--;) {

		//Cache the first movement of the list;
		linked_element_t *movement = llist_remove_first(trajectories);

		//Delete the movement;
		trajectory_delete((trajectory_t *) movement);

	}

	//If the current movement exists :
	if (tcontroller->current_movement) {

		//Delete it;
		trajectory_delete(tcontroller->current_movement);

		//Nullify it;
		tcontroller->current_movement = 0;

	}

	//Clear the movements buffer;
	cbuffer_discard_all(&tcontroller->movements);

	//TODO RESET PHYSICS LAYER

}

//TODO PAUSE PATCH:
//void tcontroller_set_pause_point(tcontroller_t *controller, bool (*restart_flag)());


//--------------------------------------- Functions called by the actuation layer --------------------------------------


/*
 * tcontroller_get_movement : returns the first movement available;
 */

movement_t *tcontroller_get_movement(tcontroller_t *const controller) {

	//If the controller is stopped :
	if (controller->state == TCONTROLLER_STOPPED) {

		//Error;
		kernel_error("trajectory.c : tcontroller_get_movement : the controller is stopped.");

	}


	//TODO IF IN FINISHING STATE AND NO MORE MOVEMENTS, STOP;

	//If no more trajectories are available :
	if (!controller->trajectories.nb_elements) {

		//Cache the state;
		tcontroller_state_t state = controller->state;

		//Stop the trajectory controller;
		tcontroller_exit(controller);

		//If the state was not FINISHING, an error occurred;
		if (state != TCONTROLLER_FINISHING) {

			//Error;
			kernel_error("tcontroller.c : tcontroller_get_movement : ran out of trajectories, but trajectories "
							 "are still available;");

		} else {

			//Notify the actuation layer not to reprogram the interrupt;
			return 0;

		}

	}

	//Return the first available sub_movement; It is not discarded for instance;
	return (movement_t *) cbuffer_read_output(&controller->movements, 0);

}


/*
 * tcontroller_discard_movement : discards the first movement, and
 * 	 updates the number of step processes that will be called until the next movement query;
 */

void tcontroller_discard_movement(tcontroller_t *const controller, const uint16_t mv_steps) {

	//Discard the first movement. If no more is available, an error will be raised;
	cbuffer_discard_output(&controller->movements);


	//TODO UPDATE MV_STEPS AFTER STEP_PROCESS IS DONE;

}

//Execute a step in the processing of the next movement;
void tcontroller_step_process(tcontroller_t *trajectory) {


	//Step 1 : Get a new position to reach
	sub_movement_data_t *sub_movement_data = SubMovementManager::read_next_sub_movement();

	//Initialise the step_distances array we will give to the kernel;
	uint8_t *elementary_distances = sub_movement_data->step_distances;

	//Copy the direction signature in cache;
	next_direction_signature = sub_movement_data->direction_signature;

	//Step 2 : Update the end_distances with this step_distances array and compute the heuristic step_distances to jerk/end points;
	SubMovementManager::update_end_jerk_distances(next_direction_signature, elementary_distances);

	//Wait for the next timer overflow;
	STEP_AND_WAIT


	//-------------------Kinematics call-------------------

	//Give the hand to the kernel who will compute the time_us for the sub-movement;
	float time_us = Kinematics::compute_us_time_for_sub_movement(sub_movement_data);

	//Wait for the next timer overflow;
	STEP_AND_WAIT


	//-------------------Signature extraction-------------------

	//Compute the step_signatures corresponding to the step_distances given by the kernel;
		process_signatures(elementary_distances, step_signatures);

	//Determine the step_period_us time_us for the next sub_movement;
	delay_us = time_us / (float) trajectory_index;

	//Discard the current sub_movement in the sub-trajectories queue;
	SubMovementManager::discard_sub_movement();

	//If no more pre-process_t is required :
	if (SubMovementManager::is_movement_processed()) {

		//Go to the end
		goto exit;

	}

	//Wait for the next timer overflow;
	STEP_AND_WAIT


	//-------------------Sub-trajectories pre-computation-------------------

	//Process a first movement for the one we made;
	SubMovementManager::push_new_sub_movement();

	//If no more pre-process_t is required;
	if (SubMovementManager::is_movement_processed()) {
		goto exit;
	}

	//Wait for the next timer overflow;
	STEP_AND_WAIT

	//Process a second movement, to fill the queue if needed;
	SubMovementManager::push_new_sub_movement();


}