//
// Created by root on 7/6/18.
//

#include "data_host.h"

#include <string.h>

#include <kernel/kernel.h>

void dhost_initialise_element(dhost_t *const dhost, const void *const data, const size_t data_size) {

	//Require a space in the node's data;
	dhost_element_t *element = dhost_provide_uninitialised(dhost);

	//If no space is available :
	if (!element) {

		//Kernel error, network not correctly dimensioned;
		kernel_error("cnetwork.c : cnetwork_activate : no space left on destination node;");

	}

	//If sizes are not compatible :
	if (data_size != dhost->data_size) {

		//Kernel error, bad node linking;
		kernel_error("cnetwork.c : cnetwork_activate : incompatible argument sizes;");

	}

	//Copy data from provided args to dest args;
	memcpy(element->data, data, data_size);


	//Provide data to the node's host;
	dhost_receive_initialised(dhost, element);

}