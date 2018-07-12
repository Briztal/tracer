/*
  computation_network.c Part of TRACER

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


#include <data_structures/containers/llist.h>
#include <kernel/kernel.h>
#include <string.h>
#include <kernel/connection/connection_t.h>
#include <data_structures/containers/instance.h>
#include "computation_network.h"


//-------------------------------------------------- Private headers  --------------------------------------------------

//Initialise the header;
void cnetwork_initialise(cnetwork_t *cnetwork);

//Find an executable node; Purely indicative; Concurrency not supported;
cnode_t *cnetwork_find_executable_node(const cnetwork_t *cnetwork);

//Insert a node in the cnetwork's active nodes list; Concurrency not supported;
void cnetwork_insert_node(cnetwork_t *cnetwork, cnode_t *new_node);


//-------------------------------------------------- Mutual exclusion --------------------------------------------------

//Shortcut for locking the node's data;
inline void cnode_lock_data(const cnode_t *const node) {

	//Lock the node;
	mutex_lock(node->data_mutex);

}


//Shortcut for unlocking the node's data;
inline void cnode_unlock_data(const cnode_t *const node) {

	//Lock the node;
	mutex_unlock(node->data_mutex);

}


//Shortcut for locking the network list;
inline void cnetwork_lock_list(const cnetwork_t *const network) {

	//Lock the node;
	mutex_lock(network->list_mutex);

}


//Shortcut for unlocking the network's list;
inline void cnetwork_unlock_list(const cnetwork_t *const network) {

	//Lock the node;
	mutex_unlock(network->list_mutex);

}


//------------------------------------------------ Creation - deletion -------------------------------------------------

/*
 * cnetwork_create : creates a computation network structure in the heap, providing the number of nodes,
 * 	and a mutex to protect the active nodes list;
 */

cnetwork_t *cnetwork_create(size_t nb_nodes, mutex_t *mutex) {

	//Allocate some heap data to contain all nodes;
	cnode_t *nodes = kernel_malloc(nb_nodes * sizeof(cnode_t));

	//Initialise all nodes to 0; Initialisation flags will be cleared;
	memset(nodes, 0, sizeof(cnode_t));

	//Create the initializer for the computation network;
	cnetwork_t init = {

		//Save the number of nodes;
		.nb_nodes = nb_nodes,

		//Initialise the list mutex;
		.list_mutex = mutex,

		//Initialise the active linked list to an empty one;
		.active_nodes = EMPTY_LINKED_LIST(nb_nodes),

		//Transfer the ownership of the nodes array;
		.nodes = nodes,

	};

	//Create and initialise the computation network;
	return kernel_malloc_copy(sizeof(cnetwork_t), &init);

}


/*
 * cnetwork_init_node : initialises a computation node, providing
 * 	- a priority;
 * 	- a mutex;
 * 	- a max number of concurrent executions;
 * 	- a number of output references;
 * 	- an array containing them;
 *
 * 	The node must not be initialised ! If so, an error will be thrown;
 */

void cnetwork_init_node(cnetwork_t *const cnetwork, const size_t node_index, const size_t priority,
						const computation_function_t function,
						const size_t arguments_size, const size_t nb_concurrent_execs,
						const size_t nb_outputs, const size_t *const output_references_const) {

	//Cache the node pointer;
	cnode_t *const node = cnetwork->nodes + node_index;

	//If the node is already initialised :
	if (node->initialised) {

		//Error. A node can't be initialised multiple times;
		kernel_error("commputation_network : cnetwork_init_node : attempted to initialise a node twice;");

	}

	//Duplicate the output references array;
	const size_t *const output_references = kernel_malloc_copy(nb_outputs * sizeof(size_t), output_references_const);


	//Initialize the node;
	*node = (cnode_t) {

		//Mark the node initialised;
		.initialised = true,

		//Reset its link;
		.link = EMPTY_LINKED_ELEMENT(),

		//Set its priority
		.priority = priority,

		//Duplicate the list mutex and save it;
		.data_mutex = mutex_duplicate(cnetwork->list_mutex),

		//Don't initialise the node's host;

		//The function, to call when the host has initialised data;
		.function = function,

		//Save the number of outputs
		.nb_output_nodes = nb_outputs,

		//Transfer the ownership of the output references array;
		.output_references = output_references

	};

	//Initialise the data host;
	dhost_initialise(&node->input_host, nb_concurrent_execs, arguments_size);

	//Decrement the number of uninitialised nodes;
	(cnetwork->uninitialised_nodes)--;

}


//Delete a computation network and all its dynamic data;
void cnetwork_delete(cnetwork_t *cnetwork) {

	//We must delete properly each node;

	//Cache the node pointer;
	cnode_t *node_p = cnetwork->nodes;

	//For each node :
	for (size_t node_index = cnetwork->nb_nodes; node_index--;) {

		//If the node is initialised :
		if (node_p->initialised) {

			//Delete the mutex;
			mutex_delete(node_p->data_mutex);

			//Free the output references array;
			kernel_free((void *) node_p->output_references);

			//Deinit the input host;
			dhost_delete(&node_p->input_host);

		}

		//update the node pointer;
		node_p++;

	}

	//Free the nodes array;
	kernel_free(cnetwork->nodes);

	//Free the cnetwork array;
	kernel_free(cnetwork);

	//The initialisation data is not owned by us, we must not delete it;

}


//----------------------------------------------- Execution / activation -----------------------------------------------


/*
 * cnetwork_execute : attempts to execute an available node;
 *
 * 	When it returns false, the computation is over and all data has been destroyed;
 */

bool cnetwork_execute(cnetwork_t *const cnetwork) {

	//If the computation is over, and data has been terminated :
	if (cnetwork->terminated) {

		//Notify that data is terminated;
		return false;

	}

	//Lock the active nodes list;
	cnetwork_lock_list(cnetwork);                                        //	List locked;

	//Find an executable node;
	cnode_t *const node = cnetwork_find_executable_node(cnetwork);       //	List locked;

	//Unlock the active nodes list;
	cnetwork_unlock_list(cnetwork);                                      //	List unlocked;


	//If no nodes were found, and the computation was not over when we entered :
	if (!node) {

		//We do not know if the computation is over yet;
		return true;

	}


	//Cache the node's data host pointer;
	dhost_t *host = &node->input_host;

	//Node may have been modified between list unlock and node lock.
	//Now that we own it, we must check if it can be executed;

	//Lock the node's data:
	cnode_lock_data(node);                                               //	Node locked;

	//Attempt to own an initialised arg ptr;
	dhost_element_t *element = dhost_provide_initialised(host);          //	Node locked;

	//Unlock the node;
	cnode_unlock_data(node);                                             //	Node unlocked;

	//If no initialised args exist :
	if (!element) {

		//We do not know if the computation is over yet;
		return true;

	}


	//Create the adequate activation structure;
	node_activation_t activator = {

		//Provide the network pointer;
		.cnetwork = cnetwork,

		//Provide the nodes reference;
		.nodes_references = node->output_references,

	};


	//Execute the node's function, providing activation structure and  owned data;
	bool node_complete = (*(node->function))(&activator, element->data);

	//Lock the node for data return;
	cnode_lock_data(node);                                                    //	Node locked;

	//If the node completed :
	if (node_complete) {                                                        //	Node locked;

		//Return data in uninitialised state;
		dhost_receive_uninitialised(host, element);                            //	Node locked;


	} else {                                                                    //	Node locked;

		//Return data in uninitialised state;
		dhost_receive_initialised(host, element);                                //	Node locked;


	}                                                                        //	Node locked;


	/*
	 * If we just did the node's final execution (all its data has been
	 * returned in the un-initialised state), we can remove it from the
	 * active list;
	 * This part is safe as data-retribution and node removal are made in the
	 * same mutex protection section;
	 */

	if (dhost_all_data_uninitialised(host)) {                                       //	Node locked;

		//Lock the active nodes list;
		cnetwork_lock_list(cnetwork);                                               //	Node locked; List locked;

		//Remove the node from the list;
		llist_remove_element(&cnetwork->active_nodes, (linked_element_t *) node);   //	Node locked; List locked;

		//Unlock the active nodes list;
		cnetwork_unlock_list(cnetwork);                                             //	Node locked; List unlocked;

	}

	//Unlock the node;
	cnode_unlock_data(node);                                                        //	Node unlocked;



	/*
	 * If there are no more active nodes, the computation must eventually be re-initialised,
	 * 	or its data must be terminated;
	 *
	 * 	As this function supports concurrency, the re-init and deletion must be safely executed;
	 */

	//Critical section; Starts here because nb_elements will be updated at reinit;
	kernel_enter_critical_section();

	//If the computation is over, and data has been terminated :
	if (cnetwork->terminated) {

		//Notify that data is terminated;
		return false;

	}

	//If there are still active nodes, we can complete here;
	if (cnetwork->active_nodes.nb_elements) {

		//Leave the critical section, only our instance will go here;
		kernel_leave_critical_section();

		return true;
	}


	//If the init/exit is already happening :
	if (cnetwork->init_exit_lock) {

		//Nothing to do, just leave the critical section;
		kernel_leave_critical_section();

		//We can't be sure that the deletion is made yet.
		return true;
	}

	//If the init/exit is not happening yet, we will do it. Lock;
	cnetwork->init_exit_lock = true;

	//Leave the critical section, only our instance will go here;
	kernel_leave_critical_section();

	//If the node must be re-initialised :
	if (cnetwork->re_initialisation_required) {

		//Re-initialise and update the re-initialisation flag;
		cnetwork_initialise(cnetwork);

		//Unlock the init/exit;
		cnetwork->init_exit_lock = false;

		//The network is re-initialised;
		return true;


	} else {

		//Mark the computation terminated;
		cnetwork->terminated = true;

		//Notify that the data is terminated;
		return false;

	}

}


void cnetwork_initialise(cnetwork_t *cnetwork) {

	//Call the initialisation function and update the initilisation flag;
	cnetwork->re_initialisation_required = (*(cnetwork->init))(cnetwork->init_data, cnetwork->nodes);

}


/*
 * cnetwork_find_executable_node : this function will traverse the portion of the list that has the highest priority (list
 * 	if supposed sorted) until it finds an executable node. If it doesn't, it returns 0;
 *
 * 	This function gives a purely indicative result. It simply states that the returned node had initialised data
 * 	available, when the function focused on it. If the node is not locked, this result may and will (murphy) have
 * 	changed when the node's execution will be attempted. As a consequence, a lock-check must be made at execution time;
 *
 * 	This function doesn't support concurrent access.
 */

cnode_t *cnetwork_find_executable_node(const cnetwork_t *const cnetwork) {

	//Cache the active list;
	linked_list_t *active_nodes = &cnetwork->active_nodes;

	//If the list comprises no nodes, return 0;
	if (!active_nodes->nb_elements) {
		return 0;
	}

	//Cache the first element;
	cnode_t *node = (cnode_t *) active_nodes->first;

	//Cache its priority;
	const size_t priority = node->priority;

	//While an executable node has not been found, and nodes can be evaluated :
	while (true) {

		//If the current node has not the highest priority :
		if (node->priority != priority) {

			//Complete, no nodes can be executed;
			return 0;

		}

		//If the data host has initialised data available :
		if (dhost_initialised_data_available(&node->input_host)) {

			//Return the node pointer;
			return node;

		}

		//update the current node;
		node = (cnode_t *) node->link.next;

		//If the previous node was the last of the list :
		if (!node) {

			//No more nodes to evaluate, no nodes are executable;
			return 0;

		}

	}


}


//Activate a neighbor node;
void cnetwork_activate_neighbor(const void *const activator, const size_t neighbor_id, const void *const args,
					   const size_t args_size) {

	//Cache the cnetwork;
	cnetwork_t *const cnetwork = ((node_activation_t *) activator)->cnetwork;

	//Determine the required node's index;
	size_t node_index = ((node_activation_t *) activator)->nodes_references[neighbor_id];

	//Cache the node;
	cnode_t *const node = cnetwork->nodes + node_index;

	//Activate the node;
	cnetwork_activate(cnetwork, node, args, args_size);

}


//Activate a node providing args;
void cnetwork_activate(cnetwork_t *const cnetwork, cnode_t *node, const void *const args, const size_t args_size) {

	//Cache the node's host;
	dhost_t *host = &node->input_host;

	//Lock the node's data;
	cnode_lock_data(node);                                                //	Node locked;

	//Determine if the node must be inserted in the list;
	bool to_insert = dhost_all_data_uninitialised(host);                //	Node locked;

	//Get an empty element, fill it and mark it initialised;
	dhost_initialise_element(host, args, args_size);                    //	Node locked;

	//Unlock the node;
	cnode_unlock_data(node);                                            //	Node unlocked;


	/*
	 * If the node must be inserted in the available queue :
	 * will happen one time, node can't be executed before being
	 * inserted in the queue, no execution concurency, and initialised
	 * state will remain until insertion in queue;
	 * Node can't be either inserted two time, because any function attempting
	 * to activate the node will now see him as partially initialised;
	 * -> to_insert will be false;
	 */

	if (to_insert) {

		//Lock the active nodes list;
		cnetwork_lock_list(cnetwork);                                    //	List locked;

		//Insert the node in the list;
		cnetwork_insert_node(cnetwork, node);                            //	List locked;

		//Unlock the list;
		cnetwork_unlock_list(cnetwork);                                //	List unlocked;

	}

}


/*
 * cnetwork_insert_node : this function traverses the active nodes list until it finds an node with a lower priority
 * 	than the provided one's, and inserts the provided node before it;
 *
 * 	If if doesn't find one, it inserts the provided node at the end of the list;
 *
 * 	This function does not support concurrency. Active list must be locked at run time;
 */

void cnetwork_insert_node(cnetwork_t *const cnetwork, cnode_t *const new_node) {

	//Cache the provided new_node's priority;
	const size_t priority = new_node->priority;

	//Cache the active list;
	linked_list_t *active_list = &cnetwork->active_nodes;

	//Cache the first new_node;
	cnode_t *current_node = (cnode_t *) active_list->first;

	//Insertion loop :
	do {

		//If the priority limit has been reached :
		if (current_node->priority >= priority) {

			//Insert the new_node before the current (if same priority, inserted before);
			llist_insert_before(active_list, (linked_element_t *) current_node, (linked_element_t *) new_node);

			//Complete here;
			return;

		}

		//If the priority limit is not reached, update the current new_node;
		current_node = (cnode_t *) current_node->link.next;

		//While the current node exists;
	} while (current_node);


	//If we traversed the whole list without reaching the limit, we must insert the new node at the list's end;
	llist_insert_last(active_list, (linked_element_t *) new_node);

}

