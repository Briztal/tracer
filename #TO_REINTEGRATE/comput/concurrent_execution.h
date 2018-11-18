/*
  concurrent_execution.h Part of TRACER

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
#ifndef TRACER_CONCURRENT_EXECUTION_H
#define TRACER_CONCURRENT_EXECUTION_H


#include "computation_node.h"


//A concurrent function realises a comput on base args;
typedef cnode_computation_state_t (*cnc_node_function_t)(void *args);

/*
 * An execution node is composed of the following elements :
 */
typedef struct concurrent_node_t {

	//A comput node, first for pointer cast;
	cmp_node_t node;

	//A data set host, to contain multiple arguments;
	dshost_t dshost;

	//A function to execute;
	const cnc_node_function_t function;
	
} concurrent_node_t;


//------------------------------------------------- Creation - deletion ------------------------------------------------

//Create and initialise a node in the heap;

concurrent_node_t *
concurrent_node_create(size_t data_element_size, size_t nb_elements, cnc_node_function_t function, mutex_t *mutex);

//----------------------------------------------- Computation node base ------------------------------------------------

//Execute a comput stage;
cnode_computation_state_t concurrent_node_compute(concurrent_node_t *concurrent_node);

//Add some data to the node; Error if data storage already initialised;
void concurrent_node_accept_data(concurrent_node_t *node, const void *args, size_t data);

//Is the node terminated ? Concurrent safe, fail-safe, purely indicative;
bool concurrent_node_terminated(concurrent_node_t *node);

//Delete a concurrent execution node;
void concurrent_node_destructor(concurrent_node_t *node);


#endif //TRACER_CONCURRENT_EXECUTION_H
