//
// Created by root on 7/8/18.
//

#include <data_structures/containers/llist.h>
#include <kernel/kernel.h>
#include <string.h>
#include <kernel/connection/connection_t.h>
#include "computation_network.h"


//-------------------------------------------------- Private headers  --------------------------------------------------

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


//----------------------------------------------- Execution / activation -----------------------------------------------


/*
 * cnetwork_execute : attempts to execute an available node;
 *
 * 	If no more nodes are available after the execution's termination, true is returned;
 */

bool cnetwork_execute(cnetwork_t *const cnetwork) {

	//Lock the active nodes list;
	cnetwork_lock_list(cnetwork);                                        //	List locked;

	//Find an executable node;
	cnode_t *const node = cnetwork_find_executable_node(cnetwork);       //	List locked;

	//Unlock the active nodes list;
	cnetwork_unlock_list(cnetwork);                                      //	List unlocked;


	//If no nodes were found, go to the end;
	if (!node) {
		goto end;//-------------------------------------------------------	GOTO --------------------------------------
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

	//If no initialised args exist, jump to the end;
	if (!element) {
		goto end;//--------------------------------------------------------	GOTO --------------------------------------
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
	cnode_lock_data(node);                                              		//	Node locked;

	//If the node completed :
	if (node_complete) {                                                		//	Node locked;

		//Return data in uninitialised state;
		dhost_receive_uninitialised(host, element);                  			//	Node locked;


	} else {                                                            		//	Node locked;

		//Return data in uninitialised state;
		dhost_receive_initialised(host, element);                    			//	Node locked;


	}                                                                   		//	Node locked;


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


	//---------------------------------------------------------------------------------	END ----------------------------

	end :

	//Return true if no nodes are in the list anymore; No mutex required, because read only, and
	//when 0, no nodes are active anymore -> no nodes can become active;
	return (cnetwork->active_nodes.nb_elements == 0);

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
	while(true) {

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


//Activate a node providing args;
void cnetwork_activate(const void *const activator, const size_t neighbor_id, const void *const args,
					   const size_t args_size) {

	//Cache the cnetwork;
	cnetwork_t *const cnetwork = ((node_activation_t *)activator)->cnetwork;

	//Determine the required node's index;
	size_t node_index = ((node_activation_t *)activator)->nodes_references[neighbor_id];

	//Cache the node;
	cnode_t *node = cnetwork->nodes + node_index;

	//Cache the node's host;
	dhost_t *host = &node->input_host;

	//Lock the node's data;
	cnode_lock_data(node); 								 				//	Node locked;

	//Determine if the node must be inserted in the list;
	bool to_insert = dhost_all_data_uninitialised(host);				//	Node locked;

	//Get an empty element, fill it and mark it initialised;
	dhost_initialise_element(host, args, args_size);					//	Node locked;

	//Unlock the node;
	cnode_unlock_data(node);                                           	//	Node unlocked;


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
		cnetwork_lock_list(cnetwork);									//	List locked;

		//Insert the node in the list;
		cnetwork_insert_node(cnetwork, node);							//	List locked;

		//Unlock the list;
		cnetwork_unlock_list(cnetwork);                     			//	List unlocked;

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
		current_node = (cnode_t *)current_node->link.next;

		//While the current node exists;
	} while(current_node);


	//If we traversed the whole list without reaching the limit, we must insert the new node at the list's end;
	llist_insert_last(active_list, (linked_element_t *) new_node);

}

