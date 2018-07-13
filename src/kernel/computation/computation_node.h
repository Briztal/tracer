//
// Created by root on 7/13/18.
//

#ifndef TRACER_COMPUTATION_NODE_H
#define TRACER_COMPUTATION_NODE_H

#include <stdbool.h>

#include <kernel/mutex/mutex.h>

#include <data_structures/containers/non_concurrent/llist.h>

#include "data_structures/containers/concurrent/dataset_host.h"



/*
 * The computation node is the most abstract and less specific data structure that can be manipulated during a
 * 	computation, and defines the standard function that can be used to interact with a computation node;
 *
 * 	A computation node stores arguments, and executes a computation, based on those arguments;
 *
 *  A function can be used to add some arguments in the storage, and another can be used to execute a
 * 	state in the computation that takes this data in arguments;
 *
 * 	This computation function returns a state, that can be :
 * 		- COMPLETE : the computation concerning a piece of data is over;
 * 		- PARTIAL : a stage in the computation of a piece of data is over;
 * 		- NO_EXECUTION : no stage was available;
 * 		- TERMINATED : the node has no data available;
 */


typedef enum {

	//The computation concerning a piece of data is over;
	CNODE_COMPLETE,

	//A stage in the computation of a piece of data is over;
	CNODE_INCOMPLETE,

	//No stage was available;
	CNODE_NOT_COMPUTED,

	//All data has been computed;
	CNODE_TERMINATED,

} cnode_computation_state_t;


typedef struct cnode_t {

	//Add some data to the data host;
	void (*const accept_data)(struct cnode_t *node);

	//Execute a computation if possible; Returns true if it did;
	cnode_computation_state_t (*const compute)(struct cnode_t *node);

	//Is the node terminated ?
	bool (*const terminated)(struct cnode_t *node);

} cnode_t;


//Shortcut for data transmission
inline void cnode_accept_data(cnode_t *node) {
	(*(node->accept_data))(node);
}

//Shortcut for computation execution
inline bool cnode_compute(cnode_t *node) {
	return (*(node->compute))(node);

}

//Shortcut for termination verification;
inline bool cnode_terminated(cnode_t *node) {
	return (*(node->terminated))(node);
}

#endif //TRACER_COMPUTATION_NODE_H
