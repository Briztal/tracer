/*
  single_execution.c Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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

#include "single_execution.h"

#include <kernel/syscall.h>



//------------------------------------------------------ Creation ------------------------------------------------------

/*
 * single_node_create : creates, initialises and returns a single node in the heap, with a data host
 * 	that stores data of the required size;
 */

single_node_t *single_node_create(const size_t data_storage_size, const snode_function_t function,
								  mutex_t *const mutex) {

	//Create an initializer;
	single_node_t init = {

		//Save function pointers;
		.node = {
			.accept_data = (void (*)(struct cmp_node_t *, void *, size_t)) &single_node_accept_data,
			.compute = (cnode_computation_state_t (*)(struct cmp_node_t *)) &single_node_compute,
			.terminated = (bool (*)(struct cmp_node_t *)) &single_node_terminated,
			.destructor = (void (*)(struct cmp_node_t *)) &single_node_destructor,
		},

		//Create an empty data host;
		.dhost = {},

		//Save the function pointer;
		.function = function

	};

	//Initialise the data host;
	dhost_initialise(&init.dhost, data_storage_size, mutex);

	//Create and initialise the node in the heap and return it;
	return kernel_malloc_copy(sizeof(single_node_t), &init);

}


//----------------------------------------------- Computation node base ------------------------------------------------

/*
 * single_node_accept_data : calls the initialisation function on the node's host. Error is raised if the hosts already
 * 	hosts data;
 */

void single_node_accept_data(single_node_t *const node, const void *const data, const size_t size) {

	//Initialise the hosts data storage; Error will be raised if not empty;
	dhost_initialise_data(&node->dhost, data, size);

}


/*
 * single_node_compute : executes the function passing stored data, if it is initialised and available.
 * If data is not initialised or not owned by the host, it returns NOT_COMPUTED;
 */

cnode_computation_state_t single_node_compute(single_node_t *const single_node) {

	//Cache the data host;
	dhost_t *const dhost = &single_node->dhost;

	//First, own some data from the host;
	void *const data = dhost_provide_initialised(dhost);

	//If no element was initialised :
	if (!data) {

		//Complete here, nothing executed;
		return CNODE_NOT_COMPUTED;

	}

	//Execute the function;
	cnode_computation_state_t state = (*(single_node->function))(single_node, data);

	//If the comput is INCOMPLETE, and data remains initialised;
	const bool still_initialised = (state == CNODE_INCOMPLETE);

	/*
	 * We must alse correct the state to reflect the node's status;
	 * The status we will return is exactly the same as the function, except for the COMPLETE status, that turns
	 * to TERMINATED because we only own one data argument;
	 */
	if (state == CNODE_COMPLETE)
		state = cmp_node_tERMINATED;

	//Give back the ownership of the data to the host;
	dhost_receive_ownership(dhost, still_initialised);

	//Return our state;
	return state;

}

/*
 * single_node_terminated : asserts if the node is terminated (contains no data anymore);
 *
 * Fail-safe, concurrency supported, unreliable results, purely indicative;
 */

bool single_node_terminated(single_node_t *const node) {

	//The node is terminated if the hosts contains no data;
	return dhost_empty(&node->dhost);

}


/*
 * single_node_destructor : deletes all dynamic data and deletes the heap node structure;
 */

void single_node_destructor(single_node_t *const node) {

	//Delete the host;
	dhost_delete(&node->dhost);

	//Delete the node;
	kernel_free(node);

}
