//
// Created by root on 7/6/18.
//

#ifndef TRACER_SYNCRONISED_QUEUE_H
#define TRACER_SYNCRONISED_QUEUE_H

#include "data_structures/containers/vlarray.h"



#include <stddef.h>


/*
 *
 */

typedef struct dhost_element_t {

	//Has the element's ownership been transferred ?
	bool ownership_transferred;

	//The next element;
	struct dhost_element_t *next;

	//The pointer to arguments;
	void *data;

} dhost_element_t;


/*
 * A dhost contains two internal queues;
 *
 * 	An internal queue contains two pointers, referencing first and last elements;
 */

typedef struct {

	//The first element of the queue; The next to be read;
	dhost_element_t *first_element;

	//The last element of the queue; A new element will be concatenated to it;
	dhost_element_t *last_element;

} dhost_queue_t;


/*
 * A synchronised queue contains an array of elements, and two internal queues, that store references to
 * 	empty and available elements that it has the ownership of;
 */

typedef struct {

	//The size of data;
	const size_t data_size;

	//The pointer to our data array;
	void *data;

	//The pointer to our elements array;
	dhost_element_t *elements;

	//The initialised elements internal queue;
	dhost_queue_t initialised_elements;

	//The initialised elements internal queue;
	dhost_queue_t uninitialised_elements;

} dhost_t;


//Create a data host, with the owning [length] elements of [element_size] bytes; Concurrency not supported;
void dhost_initialise(dhost_t *dhost, size_t nb_elements, size_t element_size);

//Delete a data host; Concurrency not supported;
void dhost_delete(dhost_t dhost);


//Is there initialised data available ? Purely indicative, concurrency supported;
bool dhost_initialised_data_available(const dhost_t *dhost);

//Is all data in the uninitialised state? Purely indicative, concurrency supported;
bool dhost_all_data_uninitialised(const dhost_t *dhost);


//Get the ownership of an uninitialised element; Concurrency not supported;
dhost_element_t *dhost_provide_uninitialised(const dhost_t *dhost);

//Get the ownership of an available element; Concurrency not supported;
dhost_element_t * dhost_provide_initialised(const dhost_t *dhost);


//Give back the ownership of element, and insert it in the uninitialised internal queue; Concurrency not supported;
void dhost_receive_uninitialised(const dhost_t *dhost, const dhost_element_t *data);

//Give back the ownership of element, and insert it in the initialised internal queue; Concurrency not supported;
void dhost_receive_initialised(const dhost_t *dhost, const dhost_element_t *data_ptr);


//Own an element to initilise, copy data inside, and give it back to the host as an initialised element;
void dhost_initialise_element(dhost_t *dhost, const void *data, size_t data_size);


#endif //TRACER_SYNCRONISED_QUEUE_H
