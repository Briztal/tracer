/*
  computation_node.h Part of TRACER

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

#ifndef TRACER_COMPUTATION_NODE_H
#define TRACER_COMPUTATION_NODE_H

#include <stdbool.h>

#include <kernel/mutex/mutex.h>

#include <data_structures/containers/non_concurrent/llist.h>

#include "data_structures/containers/concurrent/dataset_host.h"



/*
 * The comput node is the most abstract and less specific data structure that can be manipulated during a
 * 	comput, and defines the standard function that can be used to interact with a comput node;
 *
 * 	A comput node stores arguments, and executes a comput, based on those arguments;
 *
 *  A function can be used to add some arguments in the storage, and another can be used to execute a
 * 	state in the comput that takes this data in arguments;
 *
 * 	This comput function returns a state, that can be :
 * 		- COMPLETE : the comput concerning a piece of data is over;
 * 		- PARTIAL : a stage in the comput of a piece of data is over;
 * 		- NO_EXECUTION : no stage was available;
 * 		- TERMINATED : the node has no data available;
 */


typedef enum {

	//The comput concerning a piece of data is over;
	CNODE_COMPLETE,

	//A stage in the comput of a piece of data is over;
	CNODE_INCOMPLETE,

	//No stage was available;
	CNODE_NOT_COMPUTED,

	//All data has been computed;
	cmp_node_tERMINATED,

} cnode_computation_state_t;

/*
 * The comput node structure is composed of :
 */
typedef struct cmp_node_t {

	//A constant function pointer to provide data to the node;
	//Will raise an error if the node's data storage is full. You must carefully dimension your comput scheme;
	void (*const accept_data)(struct cmp_node_t *node, void *data, size_t size);

	//A constant function pointer to execute a stage in the node's comput scheme;
	// Fail-safe in itself, the stage that is executed can fail though;
	// Returns a state that describes the computed stage;
	cnode_computation_state_t (*const compute)(struct cmp_node_t *node);

	//A constant function pointer, asserting if the node is terminated.
	// Purely indicative, fail-safe, concurrency supported, unreliable results;
	bool (*const terminated)(struct cmp_node_t *node);

	//The deletion function; As the node is a simple interface, a function must be included to correctly delete it;
	// Concurrency not supported, will raise an error if the node is still in execution; Must be ran with precautions;
	void (*const destructor)(struct cmp_node_t *node);

} cmp_node_t;


//Shortcut for data transmission
inline void cnode_accept_data(cmp_node_t *const node, void *const data, const size_t size) {
	(*(node->accept_data))(node, data, size);
}

//Shortcut for comput execution
inline bool cnode_compute(cmp_node_t *const node) {
	return (*(node->compute))(node);

}

//Shortcut for termination verification;
inline bool cmp_node_terminated(cmp_node_t *const node) {
	return (*(node->terminated))(node);
}

//Shortcut for deletion; Calls the deleter;
inline void cnode_delete(cmp_node_t *const node) {
	(*(node->destructor))(node);
}

#endif //TRACER_COMPUTATION_NODE_H
