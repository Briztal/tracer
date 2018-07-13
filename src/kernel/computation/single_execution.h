//
// Created by root on 7/13/18.
//

#ifndef TRACER_COMPUTATION_FUNCTION_H
#define TRACER_COMPUTATION_FUNCTION_H


#include <data_structures/containers/concurrent/data_host.h>
#include "computation_node.h"

/*
 * The single execution node allows the non concurrent execution of a function, with the storage of a  a single
 * argument data structure.
 */

/*
 * An execution node is composed of the following elements :
 */
typedef struct single_node_t {

	//Composed of a node, to cast pointers;
	cnode_t node;
	
	//A data host, to store arguments for the single execution;
	dhost_t data_host;

	//A function to execute, that can interact with other nodes; The node itself is passed, as hosted args
	cnode_computation_state_t (*function)(struct single_node_t *node, void *args);

} single_node_t;


/*
 * The computation function of an execution node;
 */
bool single_node_compute(single_node_t *single_node) {

	//First, own some data from the host;
	void *data = data_host_provide_initialised(&single_node->data_host);

	//If no element was executed :
	if (!data) {

		//Complete here, nothing executed;
		return false;

	}

	//Execute the function;
	cnode_computation_state_t state = (*(single_node->function))(single_node, data);

	//TODO REINSERT DEPENDING ON THE STATE;




}

#endif //TRACER_COMPUTATION_FUNCTION_H
