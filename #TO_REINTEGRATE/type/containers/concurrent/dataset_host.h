/*
  data_host.h Part of TRACER

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

#ifndef TRACER_DATA_SET_HOST_H
#define TRACER_DATA_SET_HOST_H

#include "data_structures/containers/non_concurrent/vlarray.h"



#include <stddef.h>
#include <kernel/mutex/mutex.h>


/*
 * The dataset hosts owns and grants access to data structures of a fixed (shared among them) size.
 *
 * 	All data is contained in the host. Each piece of data can be in one of the three following states :
 * 		- initialised : the data structure is filled and ready to be used; Data is not directly accessible;
 * 		- uninitialised : the data structure has already been used, and its data can't be used; Data is not
 * 			directly accessible;
 * 		- shared : the data's ownership is shared with one and only one caller.
 *
 * 	A function can request access from an uninitialised or initialised piece of data. When it gets access to it,
 * 	it passes in the shared state, and no one can access it;
 *
 * 	When the function has finished its job, it provides the data structure back, and tells the host its new state.
 * 	Data can be accessed again by another part of the code;
 */


/*
 * The dshost element contains all data related to a particular data structure;
 */

typedef struct dshost_element_t {

	//Has the element's ownership been transferred ?
	bool shared;

	//The next element;
	struct dshost_element_t *next;

	//The pointer to arguments;
	void *const data;

} dshost_element_t;


/*
 * A dshost contains two internal queues;
 *
 * 	An internal queue contains two pointers, referencing first and last elements;
 */

typedef struct {

	//The number of elements in the queue;
	size_t nb_elements;

	//The first element of the queue; The next to be read;
	dshost_element_t *first_element;

	//The last element of the queue; A new element will be concatenated to it;
	dshost_element_t *last_element;

} dshost_queue_t;


/*
 * A synchronised queue contains an array of elements, and two internal queues, that store references to
 * 	empty and available elements that it has the ownership of;
 */

typedef struct {

	//The size of data;
	const size_t elements_size;

	//The number of elements that the host manages;
	const size_t nb_elements;

	//The pointer to our data array;
	void *data;

	//The mutex to protect accesses;
	mutex_t *mutex;

	//The pointer to our elements array;
	dshost_element_t *elements;

	//The initialised elements internal queue;
	dshost_queue_t initialised_elements;

	//The initialised elements internal queue;
	dshost_queue_t uninitialised_elements;

} dshost_t;



//----------------------------------------------------- Init - prempt ----------------------------------------------------

//Create a dataset host, with the owning [length] elements of [element_size] bytes; Concurrency not supported;
void dshost_initialise(dshost_t *dshost, size_t data_element_size, size_t nb_elements, const mutex_t *mutex);

//Flush the data set; Concurrency supported, but all data must be owned;
void dhost_flush(dshost_t *dshost);

//Delete a dataset host; Concurrency not supported;
void dshost_delete(dshost_t *dshost);



//-------------------------------------------------------- Sync --------------------------------------------------------


//Is there initialised data available ? Purely indicative, may change as soon as the function returns;
bool dshost_initialised_element_available(const dshost_t *dshost);

//Is all data in the uninitialised state? Purely indicative, may change as soon as the function returns;
bool dshost_empty(const dshost_t *dshost);


//----------------------------------------------- Element initialisation -----------------------------------------------

//Own an element to initialise, copy data inside, and give it back to the host as an initialised element;
void dshost_initialise_element(dshost_t *dshost, const void *data, size_t data_size);


//--------------------------------------------------- Data provision ---------------------------------------------------

//Get the ownership of an uninitialised element; Concurrency supported;
dshost_element_t *dshost_provide_element(dshost_t *dshost, bool initialised);


//--------------------------------------------------- Data reception ---------------------------------------------------

//Give back the ownership of element, and insert it in the uninitialised internal queue; Concurrency not supported;
void dshost_receive_element(dshost_t *dshost, dshost_element_t *element, bool initialised);



#endif //TRACER_DATA_SET_HOST_H
