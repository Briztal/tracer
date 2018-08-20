/*
  concurrent_execution.c Part of TRACER

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


#include "concurrent_execution.h"

#include <kernel/syscall.h>

//------------------------------------------------------ Creation ------------------------------------------------------

//Create and initialise a node in the heap;
concurrent_node_t *
concurrent_node_create(const size_t data_element_size, const size_t nb_elements, const cnc_node_function_t function,
					   mutex_t *const mutex) {


	//Initialize the node;
	concurrent_node_t init = (concurrent_node_t) {

		//Transmit functions;
		.node = {

			//CNode function take a cmp_node_t * in first argument, which composes a concurrent node. Cast is safe
			.accept_data = (void (*const)(struct cmp_node_t *, void *, size_t)) &concurrent_node_accept_data,
			.compute = (cnode_computation_state_t (*const)(struct cmp_node_t *)) &concurrent_node_compute,
			.terminated = (bool (*const)(struct cmp_node_t *)) &concurrent_node_terminated,
			.destructor = (void (*const)(struct cmp_node_t *)) &concurrent_node_destructor,
		},

		//Leave the data set host empty for instance;
		.dshost = {},

		//Save the execution function;
		.function = function,

	};

	//Initialise the data host;
	dshost_initialise(&init.dshost, data_element_size, nb_elements, mutex);

	//Create, initialise and return the node;
	return kernel_malloc_copy(sizeof(concurrent_node_t), &init);
}


//----------------------------------------------- Computation node base ------------------------------------------------

/*
 * concurrent_node_accept_data: adds some data to the node; Error if data storage already initialised;
 */

void concurrent_node_accept_data(concurrent_node_t *const node, const void *const data, const size_t size) {

	//Sub call to the data set host;
	dshost_initialise_element(&node->dshost, data, size);

}


/*
 * exec_node_compute : The comput function of an execution node;
 *
 * 	Attempts to own an executed data element, if available, executes it, and restore it to the data set host,
 * 	eventually still initialised. Returns NOT_EXECUTED if no available.
 */

bool exec_node_compute(concurrent_node_t *const exec_node) {

	//Cache the data set host;
	dshost_t *const dshost = &exec_node->dshost;

	//First, own a initialised data element from the host;
	dshost_element_t *const element = dshost_provide_element(dshost, true);

	//If no element was executed :
	if (!element) {

		//Complete here, nothing executed;
		return false;

	}

	//Execute the function;
	cnode_computation_state_t state = (*(exec_node->function))(element->data);

	//If the comput is INCOMPLETE, the node status is incomplete, and data remains initialised;
	const bool still_initialised = (state == CNODE_INCOMPLETE);

	//Restore the element to the data set host;
	dshost_receive_element(dshost, element, still_initialised);

	/*
	 * The status we will return is exactly the same as the function, except for the COMPLETE status, that will turn
	 * 	to TERMINATED if the data set host is empty (it owns all elements and none is initialised);
	 */

	//If the state is complete, and the data set host is empty :
	if ((state == CNODE_COMPLETE) && dshost_empty(dshost)) {

		//Change the state to terminated :
		state = cmp_node_tERMINATED;

	}

	//Return the updated state;
	return state;

}


/*
 * concurrent_node_terminated : asserts if the node is terminated, ie if the data set host is currently empty;
 * Concurrent safe, fail-safe, purely indicative;
 */

bool concurrent_node_terminated(concurrent_node_t *const node) {

	//Return the tatus of the data set host;
	return dshost_empty(&node->dshost);



}


/*
 * concurrent_node_destructor : deletes the data set host of the node, and the node itself;
 */

void concurrent_node_destructor(concurrent_node_t *const node) {

	//Delete the host;
	dshost_delete(&node->dshost);

	//Delete the node;
	kernel_free(node);

}

