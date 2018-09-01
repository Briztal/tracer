/*
  data_host.c Part of TRACER

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

#include "dataset_host.h"

#include <string.h>

#include <kernel/syscall.h>


//--------------------------------------------------- Private headers --------------------------------------------------

//Remove the first element of a dshost queue, and mark it shared; Concurrency not supported;
dshost_element_t *dshost_queue_remove_first(mutex_t *mutex, dshost_queue_t *queue);

//Adds the provided element at the end of the provided queue; Concurrency not supported;
void dshost_queue_insert_element(mutex_t *mutex, dshost_queue_t *queue, dshost_element_t *new_element);


//----------------------------------------------------- Init - prempt ----------------------------------------------------



/*
 * dshost_initialise : Initialises the provided data host depending on different sizes;
 *
 * 	- Allocates space for data;
 * 	- Allocates space for elements;
 * 	- Initialises and chains elements;
 * 	- Initialises the data host;
 *
 *
 * 	Concurrency is not supported;
 */

void dshost_initialise(dshost_t *dshost, const size_t data_element_size, const size_t nb_elements,
					   const mutex_t *const mutex) {

	//If the data host is not supposed to contain any element :
	if (!nb_elements) {

		//Error, the data host must contain data...
		kernel_error("dhost.c : dshost_initialise : required to initialise a data host with 0 elments;");

	}

	//Cache the size of the data array;
	const size_t data_size = nb_elements * data_element_size;

	//Allocate the data array;
	void *const data_array = kernel_malloc(data_size);

	//Allocate the elements array;
	dshost_element_t *const elements = kernel_malloc(nb_elements * sizeof(struct dshost_element_t));

	{
		//Cache the pointer to data for the current element;
		uint8_t *element_data = data_array;

		//Initialize the pointer to the element to initialise;
		dshost_element_t *current_element = elements;

		//Initialize the previously initialised element;
		dshost_element_t *previous_element = 0;

		//For each element to initialise :
		for (size_t element_index = 0; element_index < nb_elements; element_index++) {

			//Create an initializer for the element;
			const dshost_element_t init = {
				.shared = false,
				.data = element_data,
				.next = previous_element,
			};

			//Initialize the element;
			memcpy(current_element, &init, sizeof(dshost_element_t));

			//Update the data pointer to point to the next piece of data;
			element_data += data_element_size;

			//Update the previous element pointer to point to the current
			previous_element = current_element;

			//Update the current element to point to the next one;
			current_element++;

		}

	}

	//Initialise the data host;
	const dshost_t dshost_init = {

		//Save the size of an element;
		.elements_size = data_element_size,

		//Save the number of elements managed by the host;
		.nb_elements = nb_elements,

		//Data is stored in the data array;
		.data = data_array,

		//Duplicate the mutex and transfer its ownership;
		.mutex = mutex_duplicate(mutex),

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

	//Initialise the data set host;
	memcpy(dshost, &dshost_init, sizeof(dshost_t));

}


/*
 * dhost_flush : attempts to flush the data set; Concurrency supported, but all data must be owned;
 *
 */

void dhost_flush(dshost_t *dshost) {

	//Lock the struct;TODO WAIT UNTIL ALL DATA IS RETRIBUTED AND DELETE;
	//TODO MAKE A FLAG THAT PREVENTS DATA PROVISION.

	//If there still is shared data :
	if (dshost->uninitialised_elements.nb_elements + dshost->initialised_elements.nb_elements != dshost->nb_elements) {

		//Error. All data must be processed before deletion;
		kernel_error("dhost.c : dshost_delete : there still is data initialised or shared;");

	}


}


/*
 * dshost_delete : deletes all dynamic data of the data host.
 *
 * 	dost structure is not fred, as it can compose another struct;
 *
 * 	Concurrency is not supported;
 */

void dshost_delete(dshost_t *const dshost) {


	//Data and elements are contained in arrays. Simply free them;
	kernel_free(dshost->data);
	kernel_free(dshost->elements);

}


//-------------------------------------------------------- Sync --------------------------------------------------------

/*
 * dshost_initialised_element_available : this function asserts is there is initialised data available;
 *
 * 	Purely indicative, concurrency supported;
 */

bool dshost_initialised_element_available(const dshost_t *dshost) {

	//There is initialised data available if elements are linked in the initialised elements list;
	return (dshost->initialised_elements.nb_elements != 0);

}


/*
 * dshost_empty : this function asserts if all elements are linked in the uninitialised elements list;
 *
 * 	Purely indicative, concurrency supported;
 */

bool dshost_empty(const dshost_t *dshost) {

	//All data is uninitialised if all elements are linked in the uninitialised queue;
	return (dshost->uninitialised_elements.nb_elements == dshost->nb_elements);

}


//----------------------------------------------- Element initialisation -----------------------------------------------

/*
 * dshost_initialise_element : required the ownership of an element, initialises it, and gives back the ownership of
 * 	the element to the container;
 *
 * 	If no element is available, or sizes do not match, an exception is raised;
 */

void dshost_initialise_element(dshost_t *const dshost, const void *const data, const size_t data_size) {

	//Require a space in the node's data;
	dshost_element_t *element = dshost_provide_element(dshost, false);

	//If no space is available :
	if (!element) {

		//Kernel error, network not correctly dimensioned;
		kernel_error("cnetwork.c : cnetwork_activate : no space left on destination node;");

	}

	//If sizes are not compatible :
	if (data_size != dshost->elements_size) {

		//Kernel error, bad node linking;
		kernel_error("cnetwork.c : cnetwork_activate : incompatible argument sizes;");

	}

	//Copy data from provided args to dest args;
	memcpy(element->data, data, data_size);

	//Provide initialised data to the node's host;
	dshost_receive_element(dshost, element, true);

}


//--------------------------------------------------- Data provision ---------------------------------------------------

/*
 * dshost_provide_element : Attempts to remove the first element of the required queue;
 *
 * 	If it exists, it marks it shared and returns its pointer;
 *
 * 	If not, it returns 0;
 */

dshost_element_t *dshost_provide_element(dshost_t *const dshost, const bool initialised) {

	//If the element is initialised, the concerned queue is the initialised one, if not, the uninitialised one;
	dshost_queue_t *const queue = (initialised) ? &dshost->initialised_elements : &dshost->uninitialised_elements;

	//Insert the provided element at the end of the concerned list;
	return dshost_queue_remove_first(dshost->mutex, queue);

}



/*
 * dshost_queue_remove_first : Attempts to remove the first element of the provided queue;
 *
 * 	If it exists, it marks it shared and returns its pointer;
 *
 * 	If not, it returns 0;
 */

dshost_element_t *dshost_queue_remove_first(mutex_t *const mutex, dshost_queue_t *const queue) {

	//Lock the access to the queue;
	mutex_lock(mutex);

	//If the queue is empty :
	if (!queue->nb_elements) {

		//Unlock access to the queue;
		mutex_unlock(mutex);

		//Return a null pointer. Must be caught after;
		return 0;

	}

	//If the queue is not empty :

	//Cache the first element;
	dshost_element_t *element = queue->first_element;

	//Cache the element's successor;
	dshost_element_t *next = element->next;

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
		kernel_error("dhost.c : dshost_queue_remove_first : removed element is already shared;");

	}

	//Mark the element shared;
	element->shared = true;

	//Unlock access to the queue;
	mutex_unlock(mutex);

	//Return the removed element;
	return element;

}


//--------------------------------------------------- Data reception ---------------------------------------------------

/*
 * dshost_receive_uninitialised : attempts to insert the element at the end of the required list;
 */

void dshost_receive_element(dshost_t *const dshost, dshost_element_t *const element, const bool initialised) {

	//If the element is initialised, the concerned queue is the initialised one, if not, the uninitialised one;
	dshost_queue_t *const queue = (initialised) ? &dshost->initialised_elements : &dshost->uninitialised_elements;

	//Insert the provided element at the end of the concerned list;
	dshost_queue_insert_element(dshost->mutex, queue, element);

}


/*
 * dshost_queue_insert_element : if the provided element is shared, inserts it at the end of the provided list.
 */

void
dshost_queue_insert_element(mutex_t *const mutex, dshost_queue_t *const queue, dshost_element_t *const new_element) {

	//If the element is not shared, it can't be inserted :
	if (!new_element->shared) {

		//Error, element should have been shared previously;
		kernel_error("dhost.c : dshost_queue_remove_first : provided element is not shared;");

	}


	//Lock access to the queue;
	mutex_lock(mutex);

	//Mark the element not shared;
	new_element->shared = false;

	//Reset the eventual element's successor;
	new_element->next = 0;

	//If the queue is empty :
	if (!queue->nb_elements) {

		//Lazy init;
		*queue = (dshost_queue_t) {
			.nb_elements = 1,
			.first_element = new_element,
			.last_element = new_element,
		};

		//Complete here;
		return;
	}

	//If the queue is not empty :

	//Cache the queue's last element;
	dshost_element_t *last = queue->last_element;

	//Set the new element to be the successor of the last element;
	last->next = new_element;

	//Update the last element of the queue;
	queue->last_element = new_element;

	//Increment the number of elements in the queue;
	queue->nb_elements++;

	//UnLock access to the queue;
	mutex_unlock(mutex);

}



