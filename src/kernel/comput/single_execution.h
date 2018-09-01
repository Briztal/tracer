/*
  single_execution.h Part of TRACER

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

#ifndef TRACER_SINGLE_EXECUTION_H
#define TRACER_SINGLE_EXECUTION_H

#include "computation_node.h"

#include <data_structures/containers/concurrent/data_host.h>

/*
 * The single execution node allows the non concurrent execution of a function, with the storage of a  a single
 * argument data structure.
 */

struct single_node_t;

//Define the single comput function type;
typedef cnode_computation_state_t (*snode_function_t)(struct single_node_t *node, void *args);

/*
 * An execution node is composed of the following elements :
 */
typedef struct single_node_t {

	//Composed of a node, to cast pointers;
	cmp_node_t node;
	
	//A data host, to store arguments for the single execution;
	dhost_t dhost;

	//A function to execute, that can interact with other nodes; The node itself is passed, as hosted args
	const snode_function_t function;

} single_node_t;

//------------------------------------------------------ Creation ------------------------------------------------------

//Create and initialise a node in the heap;
single_node_t *single_node_create(size_t data_storage_size, snode_function_t function, mutex_t *mutex);


//----------------------------------------------- Computation node base ------------------------------------------------

//Execute a comput stage;
cnode_computation_state_t single_node_compute(single_node_t *single_node);

//Add some data to the node; Error if data storage already initialised;
void single_node_accept_data(single_node_t *node, const void *args, size_t data);

//Is the node terminated ? Concurrent safe, fail-safe, purely indicative;
bool single_node_terminated(single_node_t *node);

//Delete a single execution node;
void single_node_destructor(single_node_t *node);


#endif //TRACER_SINGLE_EXECUTION_H

