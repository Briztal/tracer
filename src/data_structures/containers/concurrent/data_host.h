//
// Created by root on 7/13/18.
//

#ifndef TRACER_RESOURCE_H
#define TRACER_RESOURCE_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include <kernel/mutex/mutex.h>

/*
 * The data host hosts a single data element
 */

typedef struct {

	//The data_storage's mutex;
	mutex_t *const mutex;

	//The data_storage initialisation flag;
	volatile bool initialised;

	//The data_storage ownership flag; Set when the data host owns the data_storage;
	volatile bool owned;

	//The data_storage's size;
	const size_t data_size;

	//The data_storage itself;
	void *const data_storage;

} dhost_t;


//----------------------------------------------------- Init - exit ----------------------------------------------------

//Initialise the data host structure;
void dhost_initialise(dhost_t *dhost, size_t size, mutex_t *mutex);


//Delete the data host structure;
void dhost_delete(dhost_t *dhost);

//----------------------------------------------------- Init - exit ----------------------------------------------------

//Is the data element initialised ?
bool dhost_data_initialised(dhost_t *dhost);


//--------------------------------------------------- Data provision ---------------------------------------------------

//Transmit the ownership of the data_storage;
void *const data_host_provide_initialised(dhost_t *dhost);


//----------------------------------------------- Element initialisation -----------------------------------------------

//Copy some data in the data storage; Size check is made;
void data_host_initialise_data(dhost_t *const dhost, const void *const data, const size_t data_size);


//--------------------------------------------------- Data reception ---------------------------------------------------

//Get back the ownership of the data element;
void data_host_receive_ownsership(dhost_t *dhost, bool initialised);


#endif //TRACER_RESOURCE_H
