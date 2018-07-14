//
// Created by root on 7/13/18.
//

#include "data_host.h"

#include <string.h>

#include <kernel/kernel.h>


//----------------------------------------------------- Init - exit ----------------------------------------------------

/*
 * dhost_initialise : allocates memory to contain a data element, initialises flags and data structure;
 */

void dhost_initialise(dhost_t *const dhost, const size_t size, mutex_t *const mutex) {

	//Allocate some space to store data;
	void *const data_storage = kernel_malloc(size);

	//Initialise the data structure;
	dhost_t init = {

		//Transfer the ownership of the mutex;
		.mutex = mutex,

		//Element not initialised;
		.initialised = false,

		//We own the element for instance;
		.owned = true,

		//Save the size;
		.data_size = size,

		//Transfer the ownership of the array;
		.data_storage = data_storage,

	};

	//Initialise the data host;
	memcpy(dhost, &init, sizeof(dhost_t));

}


/*
 * dhost_delete : deletes the data host structure; Concurency is not supported;
 */

void dhost_delete(dhost_t *const dhost) {

	//If data is not owned :
	if(!dhost->owned) {

		//Error, data must be owned before deletion;
		kernel_error("dhost : dhost_delete : data is not owned by the host, cannot delete safely;");
	}

	//Delete the data storage;
	kernel_free(dhost->data_storage);

	//Delete the mutex;
	mutex_delete(dhost->mutex);

}


//----------------------------------------------------- Init - exit ----------------------------------------------------

/*
 * dhost_data_available : asserts if the data storage owns its data and if it is not initialised;;
 *
 * 	Purely indicative, unreliable results, concurrence supported, fail-safe;
 */

bool dhost_empty(const dhost_t *const dhost) {

	const bool owned = dhost->owned, initialsed = dhost->initialised;

	//Assert if data is owned by the host, and not initialsed;
	return initialsed || (!owned);

}


//----------------------------------------------- Element initialisation -----------------------------------------------

/*
 * dhost_initialise_data : Initialises the data storage;
 *
 * 	Error is raised if sizes don't match, or if the storage is not owned, or already initialised;
 */

void dhost_initialise_data(dhost_t *const dhost, const void *const data, const size_t data_size) {

	//If sizes are incompatible :
	if (data_size != dhost->data_size) {

		//Fail, src and dest are not compatible;
		kernel_error("dhost.c : dhost_initialise_data : incompatible sizes;");

	}

	//Cache the mutex;
	mutex_t *const mutex = dhost->mutex;

	//Lock access;
	mutex_lock(mutex);

	//If data is not owned by the host:
	if (!dhost->owned) {

		//Error.
		kernel_error("dhost.c : dhost_initialise_data : data not owned by the host;");

	}

	//If data is already initialised :
	if (dhost->initialised) {

		//Error.
		kernel_error("dhost.c : dhost_initialise_data : data already initialised;");

	}

	//Data can be initialised safely;
	memcpy(dhost->data_storage, data, data_size);

	//Mark data initialised;
	dhost->initialised = true;

	//Unlock access;
	mutex_unlock(mutex);

}




//--------------------------------------------------- Data provision ---------------------------------------------------


/*
 * dhost_provide_initialised : transmits the ownership of the data_storage to the caller, if it is initialised;
 *
 * 	If not, 0 is returned;
 */

void *const dhost_provide_initialised(dhost_t *const dhost) {

	//Cache the mutex;
	mutex_t *const mutex = dhost->mutex;

	//Lock access;
	mutex_lock(mutex);

	//Declare a null pointer;
	void *data = 0;

	//If the provision is unlocked, data is owned, and initialised :
	if (dhost->owned && dhost->initialised) {

		//Update data to the address of the data storage;
		data = dhost->data_storage;

	}

	//Unlock access;
	mutex_unlock(mutex);

	//Return the cached pointer;
	return data;

}


//--------------------------------------------------- Data reception ---------------------------------------------------

/*
 * dhost_receive_ownership : get back the ownership of the data element;
 *
 * 	Error if the element is already owned;
 */

void dhost_receive_ownership(dhost_t *const dhost, const bool initialised) {

	//Cache the mutex;
	mutex_t *const mutex = dhost->mutex;

	//Lock access;
	mutex_lock(mutex);

	//If data is already owned by the host:
	if (dhost->owned) {

		//Error.
		kernel_error("dhost.c : dhost_initialise_data : data already owned by the host;");

	}

	//Mark data initialised;
	dhost->initialised = initialised;

	//Mark data owned;
	dhost->owned = true;

	//Unlock access;
	mutex_unlock(mutex);


}