//
// Created by root on 7/4/18.
//

#ifndef TRACER_POSITION_COMPUTER_H
#define TRACER_POSITION_COMPUTER_H

#include <stdint.h>

#include "machine.h"

#include "kernel/computation/staged_execution.h"

typedef struct {

	//The position target computer instance;
	void *instance;

	//Compute the position for the new machine state;
	cnode_state_t (*compute)(

		//The instance is provided; Write is allowed;
		void *instance,

		//Machine constants are provided. Read only;
		const machine_constants_t *machine,

		//The current state is provided. Read only;
		const machine_state_t *current_state,

		//The new custom state is provided. Write is allowed;
		void *new_custom_state,

		//The distances array to fill is provided. Write is allowed;
		int16_t *distances

	);

	//Notify the position computer that the previously provided position has been rejected;
	void (*reset)(void *instance);

	//Notify the position computer that the previously provided position has been accepted;
	void (*accept)(void *instance);

	//Delete the position computer. Required for cleanup;
	void (*delete)(void *instance);

} distances_computer_t;


//---------------------------------------- inline shortcuts ----------------------------------------

/*
 * Shortcut for distances computation stage;
 */

inline cnode_state_t distances_computer_compute(
	distances_computer_t *computer, const machine_constants_t *constants, const machine_state_t *current_state,
	void *const new_custom_state, int16_t *distances
) {

	//Compute the position and return the status;
	return (*(computer->compute))(computer->instance, constants, current_state, new_custom_state, distances);

}



#endif //TRACER_POSITION_COMPUTER_H
