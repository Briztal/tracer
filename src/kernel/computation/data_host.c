//
// Created by root on 7/6/18.
//

#include "data_host.h"

#include <string.h>

#include <kernel/kernel.h>


//--------------------------------------------------- Private headers --------------------------------------------------

//Remove the first element of a dhost queue, and mark it shared; Concurrency not supported;
dhost_element_t *dhost_queue_remove_first(dhost_queue_t *queue);

//Adds the provided element at the end of the provided queue; Concurrency not supported;
void dhost_queue_insert_element(dhost_queue_t *queue, dhost_element_t *new_element);


//----------------------------------------------------- Init - exit ----------------------------------------------------



/*
 * dhost_initialise : Initialises the provided data host depending on different sizes;
 *
 * 	- Allocates space for data;
 * 	- Allocates space for elements;
 * 	- Initialises and chains elements;
 * 	- Initialises the data host;
 *
 *
 * 	Concurrency is not supported;
 */

void dhost_initialise(dhost_t *dhost, const size_t nb_elements, const size_t element_data_size) {

	//If the data host is not supposed to contain any element :
	if (!nb_elements) {

		//Error, the data host must contain data...
		kernel_error("data_host.c : dhost_initialise : required to initialise a data host with 0 elments;");

	}

	//Cache the size of the data array;
	size_t data_size = nb_elements * element_data_size;

	//Allocate the data array;
	void *data_array = kernel_malloc(data_size);

	//Allocate the elements array;
	dhost_element_t *elements = kernel_malloc(nb_elements * sizeof(struct dhost_element_t));

	{
		//Cache the pointer to data for the current element;
		uint8_t *element_data = data_array;

		//Initialize the pointer to the element to initialise;
		dhost_element_t *current_element = elements;

		//Initialize the previously initialised element;
		dhost_element_t *previous_element = 0;

		//For each element to initialise :
		for (size_t element_index = 0; element_index < nb_elements; element_index++) {


			//Create an initializer for the element;
			dhost_element_t init = {
				.shared = false,
				.data = element_data,
				.next = previous_element,
			};

			//Initialize the element;
			*current_element = init;

			//Update the data pointer to point to the next piece of data;
			element_data += element_data_size;

			//Update the previous element pointer to point to the current
			previous_element = current_element;

			//Update the current element to point to the next one;
			current_element++;

		}

	}

	//Initialise the data host;
	*dhost = {

		//Save the size of an element;
		.elements_size = element_data_size,

		//Save the number of elements managed by the host;
		.nb_elements = nb_elements,

		//Data is stored in the data array;
		.data = data_array,

		//Elements are stored in the elements array;
		.elements = elements,

		//No elements are initialised for instance;
		.initialised_elements = {
			.nb_elements = 0,
			.first_element = 0,
			.last_element = 0,
		},

		//All elements are un-initialised, and have been linked in the decreasing order;
		.uninitialised_elements = {

			//The list contains all elements for instance;
			.nb_elements = nb_elements,

			//The first element has no predecessor, and is so the last element of the array;
			.first_element = elements + (nb_elements - (size_t) 1),

			//The last element has no successor (0 in "next" field) and is so the first element of the array;
			.last_element = elements,

		},

	};

}


/*
 * dhost_delete : deletes all dynamic data of the data host.
 *
 * 	dost structure is not fred, as it can compose another struct;
 *
 * 	Concurrency is not supported;
 */

void dhost_delete(dhost_t *const dhost) {

	//If there still is shared data :
	if (dhost->uninitialised_elements.nb_elements + dhost->initialised_elements.nb_elements != dhost->nb_elements) {

		//Error. All data must be processed before deletion;
		kernel_error("data_host.c : dhost_delete : there still is data initialised or shared;");

	}

	//Data and elements are contained in arrays. Simply free them;
	kernel_free(dhost->data);
	kernel_free(dhost->elements);

}


//-------------------------------------------------------- Sync --------------------------------------------------------


/*
 * dhost_initialised_data_available : this function asserts is there is initialised data available;
 *
 * 	Purely indicative, concurrency supported;
 */

bool dhost_initialised_data_available(const dhost_t *dhost) {

	//There is initialised data available if elements are linked in the initialised elements list;
	return (dhost->initialised_elements.nb_elements != 0);

}


/*
 * dhost_all_data_uninitialised : this function asserts if all elements are linked in the uninitialised elements list;
 *
 * 	Purely indicative, concurrency supported;
 */

bool dhost_all_data_uninitialised(const dhost_t *dhost) {

	//All data is uninitialised if all elements are linked in the uninitialised queue;
	return (dhost->uninitialised_elements.nb_elements == dhost->nb_elements);

}


//--------------------------------------------------- Data provision ---------------------------------------------------

/*
 * dhost_provide_uninitialised : Attempts to remove the first element of the uninitialised queue;
 *
 * 	If it exists, it marks it shared and returns its pointer;
 *
 * 	If not, it returns 0;
 */

dhost_element_t *dhost_provide_uninitialised(const dhost_t *const dhost) {

	//Remove and return the first element of the uninitialised queue;
	return dhost_queue_remove_first(&dhost->uninitialised_elements);

}


/*
 * dhost_provide_initialised : Attempts to remove the first element of the initialised queue;
 *
 * 	If it exists, it marks it shared and returns its pointer;
 *
 * 	If not, it returns 0;
 */

dhost_element_t *dhost_provide_initialised(const dhost_t *const dhost) {

	//Remove and return the first element of the initialised queue;
	return dhost_queue_remove_first(&dhost->uninitialised_elements);

}


/*
 * dhost_queue_remove_first : Attempts to remove the first element of the provided queue;
 *
 * 	If it exists, it marks it shared and returns its pointer;
 *
 * 	If not, it returns 0;
 */

dhost_element_t *dhost_queue_remove_first(dhost_queue_t *const queue) {

	//If the queue is empty :
	if (!queue->nb_elements) {

		//Return a null pointer. Must be caught after;
		return 0;

	}

	//If the queue is not empty :

	//Cache the first element;
	dhost_element_t *element = queue->first_element;

	//Cache the element's successor;
	dhost_element_t *next = element->next;

	//Reset the element's successor;
	element->next = 0;

	//Set the first element to be the successor;
	queue->first_element = next;

	//If the next element is null :
	if (!next) {

		//We must reset the last element;
		queue->last_element = 0;

	}

	//Decrease the number of elements in the queue;
	queue->nb_elements--;

	//If the element is already shared :
	if (element->shared) {

		//Error, list not supposed to contain a shared element;
		kernel_error("data_host.c : dhost_queue_remove_first : removed element is already shared;");

	}

	//Mark the element shared;
	element->shared = true;

	//Return the removed element;
	return element;

}




//--------------------------------------------------- Data reception ---------------------------------------------------

/*
 * dhost_receive_uninitialised : if the provided element is shared, inserts it at the end of the provided list.
 */

void dhost_receive_uninitialised(const dhost_t *dhost, dhost_element_t *element) {

	//Insert the provided element at the end of the uninitialised elements list;
	dhost_queue_insert_element(&dhost->uninitialised_elements, element);

}


/*
 * dhost_receive_uninitialised : if the provided element is shared, inserts it at the end of the provided list.
 */
void dhost_receive_initialised(const dhost_t *dhost, dhost_element_t *element) {

	//Insert the provided element at the end of the initialised elements list;
	dhost_queue_insert_element(&dhost->uninitialised_elements, element);

}


/*
 * dhost_queue_insert_element : if the provided element is shared, inserts it at the end of the provided list.
 */

void dhost_queue_insert_element(dhost_queue_t *const queue, dhost_element_t *const new_element) {

	//If the element is not shared, it can't be inserted :
	if (!new_element->shared) {

		//Error, element should have been shared previously;
		kernel_error("data_host.c : dhost_queue_remove_first : provided element is not shared;");

	}

	//Mark the element not shared;
	new_element->shared = false;

	//Reset the eventual element's successor;
	new_element->next = 0;

	//If the queue is empty :
	if (!queue->nb_elements) {

		//Lazy init;
		*queue = {
			.nb_elements = 1,
			.first_element = new_element,
			.last_element = new_element,
		};

		//Complete here;
		return;
	}

	//If the queue is not empty :

	//Cache the queue's last element;
	dhost_element_t *last = queue->last_element;

	//Set the new element to be the successor of the last element;
	last->next = new_element;

	//Update the last element of the queue;
	queue->last_element = new_element;

	//Increment the number of elements in the queue;
	queue->nb_elements++;

}



//----------------------------------------------- Element initialisation -----------------------------------------------



void dhost_initialise_element(dhost_t *const dhost, const void *const data, const size_t data_size) {

	//Require a space in the node's data;
	dhost_element_t *element = dhost_provide_uninitialised(dhost);

	//If no space is available :
	if (!element) {

		//Kernel error, network not correctly dimensioned;
		kernel_error("cnetwork.c : cnetwork_activate : no space left on destination node;");

	}

	//If sizes are not compatible :
	if (data_size != dhost->elements_size) {

		//Kernel error, bad node linking;
		kernel_error("cnetwork.c : cnetwork_activate : incompatible argument sizes;");

	}

	//Copy data from provided args to dest args;
	memcpy(element->data, data, data_size);


	//Provide data to the node's host;
	dhost_receive_initialised(dhost, element);

}


