/*
  circular_buffer.c - Part of TRACER

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

#include <stdbool.h>
#include <kernel/kernel.h>
#include "circular_buffer.h"
#include "vlarray.h"


/*
 * Delete the cbuffer;
 */

void cbuffer_delete(cbuffer_t *cbuffer) {

	vlarray_clear(&cbuffer->container);

}

/*
 * discard_all : discards all elements in the buffer, but doesn't change the container's size;
 */

void cbuffer_discard_all(cbuffer_t *const cbuffer) {

	//Set the number of spaces at its maximum;
	cbuffer->nb_spaces = cbuffer->container.length;

	//Set the number of elements to 0;
	cbuffer->nb_elements = 0;

	//Set input and output indices at their initial value;
	cbuffer->input_index = cbuffer->output_index = 0;

}

//----------------------------------- Input -----------------------------------

/*
 * cbuffer_insert : adds an element in the buffer, at the insertion position; Integrity guaranteed;
 */

void cbuffer_insert(cbuffer_t *const cbuffer, const void *const element_p) {

	//Spaces will be verified during cbuffer_validate_input;

	//Set the element at the correct position;
	vlarray_set_element(&cbuffer->container, cbuffer->input_index, element_p);

	//Validate the input position;
	cbuffer_validate_input(cbuffer);

}

/*
 * cbuffer_validate_input : increase the input index, and guarantee the integrity of the input element;
 */

void cbuffer_validate_input(cbuffer_t *const cbuffer) {

	//If no spaces are available, validation is impossible;
	if (!cbuffer->nb_spaces) {

		//Error;
		kernel_error("circular_buffer.c : cbuffer_validate_input : the buffer is full;");

		//Fail. Never reached;
		return;
	}

	//Increment safely;
	cbuffer->input_index = (cbuffer->input_index + (size_t) 1) % cbuffer->container.length;

	//Increment the number of elements;
	cbuffer->nb_elements++;

	//Decrement the number spaces;
	cbuffer->nb_spaces--;

}


/*
 * cbuffer_read_input : read an element from the input point, at a given offset.
 *  An offset of 0 hits the input element, 1 hits the element inserted just before, etc...
 */

void *cbuffer_read_input(const cbuffer_t *const cbuffer, const size_t offset) {

	/*
	 * read_input is used to access previously inserted data, but also to update the element we will validate;
	 * As a consequence, the reading will be invalid if the offset is strictly greater than the number of elements
	 *  in the buffer;
	 */

	if (offset > cbuffer->nb_elements) {

		//Error;
		kernel_error("circular_buffer.c : cbuffer_read_input : offset is greater than the number of elements;;");

		//Fail. Never reached;
		return 0;

	}

	//First, cache the input index;
	size_t input_index = cbuffer->input_index;

	//Then determine the index of the element to read;
	size_t index = (input_index >= offset) ? input_index - offset : cbuffer->container.length -
																	(offset - input_index);

	//Get the element at the required position. A modulo is applied to ensure to hit a container's element;
	return vlarray_get_element(&cbuffer->container, index);

}


//------------------------------------ Output ------------------------------------

/*
 * cbuffer_discard_output : discards the output element. Its integrity not guaranteed anymore;
 */

void cbuffer_discard_output(cbuffer_t *const cbuffer) {

	//If no elements are available, discarding is impossible;
	if (!cbuffer->nb_elements) {

		//Error;
		kernel_error("circular_buffer.c : cbuffer_discard_output : the buffer is empty;");

		//Fail. Never reached;
		return;

	}

	//Increment safely;
	cbuffer->output_index = (cbuffer->output_index + (size_t) 1) % cbuffer->container.length;

	//Decrement the number of elements;
	cbuffer->nb_elements--;

	//Increment the number of spaces;
	cbuffer->nb_spaces++;

}


/*
 * Read an element from the output index, at a given offset.
 * An offset of 0 hits the output element, 1 hits the element inserted right after, ...
 */

void *cbuffer_read_output(cbuffer_t *const cbuffer, const size_t offset) {

	/*
	 * read_output is used to access previously inserted data
	 * As a consequence, the reading will be invalid if the offset is greater or equal to the number of elements
	 *  in the buffer;
	 */

	if (offset >= cbuffer->nb_elements) {

		//Error;
		kernel_error("circular_buffer.c : cbuffer_read_output : offset is greater or equal"
						 " than the number of elements;");

		//Fail. Never reached;
		return 0;

	}

	//First, cache the output_index index;
	size_t output_index = cbuffer->output_index;

	//As an addition is required, we will switch to the larger type to avoid a type overflow;
	uint32_t larger_index = (uint32_t) output_index + (uint32_t) offset;

	//Then determine the index of the element to read by a modulo;
	size_t index = (uint16_t) (larger_index % (uint32_t) cbuffer->container.length);

	//Get the element at the required position. A modulo is applied to ensure to hit a container's element;
	return vlarray_get_element(&cbuffer->container, index);

}


//------------------------------------ Resize ------------------------------------

/*
 * cbuffer_resize : resizes the circular buffer.
 *  All elements pointers that have been queried are invalidated;
 */

void cbuffer_resize(cbuffer_t *const cbuffer, const size_t new_size) {

	//Dynamic resize is not supported for instance;
	if (cbuffer->nb_elements) {

		//Error;
		kernel_error("circular_buffer.c : cbuffer_resize : resizing of non empty buffer is not supported;");

		//Fail. Never reached;
		return;

	}

	//Resize the container;
	vlarray_resize(&cbuffer->container, new_size);

	//Reset its data;
	cbuffer->nb_spaces = new_size;
	cbuffer->nb_elements = 0;
	cbuffer->output_index = cbuffer->input_index = 0;

	/*
	 * First, we must determine if rearrangement will be necessary.
	 *  Elements are not stored from 0 to x, they can overlap the end of the container.
	 *
	 *  A rearrangement is required if the input index is strictly lower than the output index,
	 *      or if there are no spaces in the array;
	 */

	/* IF A DYNAMIC RESIZE IS REQUIRED : THAT'S THE BEGINNING
	 *

	//For instance, size reduction is not supported;
	if (new_size <= cbuffer->container.length) {
		return;//TODO ERROR;
	}

	bool rearrangement = (cbuffer->input_index < cbuffer->output_index) || cbuffer->nb_spaces;

	//Cache the old size of the container;
	size_t old_size =  cbuffer->container.length;

	//Cache the number of added elements;
	uint8_t added_elements =

	//Increment the number of spaces of the number of added cases;
	cbuffer->nb_spaces += new_size - cbuffer->container.length;

	//Then, resize the array;
	vlarray_resize(&cbuffer->container, new_size);

	//If rearrangement is required :
	if (rearrangement) {

		*/

}



