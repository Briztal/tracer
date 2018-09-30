//
// Created by root on 9/26/18.
//

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

#include <kernel/log.h>
#include "cbuffer.h"

#define CBUFFER_ELEMENT_POINTER(buffer, element_index)\
 ((void *) (((uint8_t *)(buffer)->array) + (element_index) * (buffer)->element_size))


/*
 * discard_all : discards all elements in the buffer, but doesn't change the container's size;
 */

void cbuffer_clear(struct cbuffer *const buffer) {
	
	//Set the number of spaces at its maximum;
	buffer->nb_spaces = buffer->length;
	
	//Set the number of elements to 0;
	buffer->nb_elements = 0;
	
	//Set input and output indices at their initial value;
	buffer->input_index = buffer->output_index = 0;
	
}

//----------------------------------- Input -----------------------------------

/*
 * cbuffer_validate_input : increase the input index, and guarantee the integrity of the input element;
 */

void cbuffer_validate_input(struct cbuffer *const buffer) {
	
	//If no spaces are available, validation is impossible;
	if (!buffer->nb_spaces) {
		
		//Error. Do nothing
		kernel_log_("circular_buffer.c : cbuffer_validate_input : no space available;");
		return;
	}
	
	//Increment safely;
	buffer->input_index = (buffer->input_index + (size_t) 1) % buffer->length;
	
	//Increment the number of elements;
	buffer->nb_elements++;
	
	//Decrement the number spaces;
	buffer->nb_spaces--;
	
}


/*
 * cbuffer_read_input : read an element from the input point, at a given offset.
 *  An offset of 0 hits the input element, 1 hits the element inserted just before, etc...
 */

void *cbuffer_get_input(const struct cbuffer *const buffer, const size_t offset) {
	
	/*
	 * read_input is used to access previously inserted data, but also to update the element we will validate;
	 * As a consequence, the reading will be invalid if the offset is strictly greater than the number of elements
	 *  in the buffer;
	 */
	
	if (offset > buffer->nb_elements) {
		
		//Error. Do nothing
		kernel_log_("circular_buffer.c : cbuffer_read_input : offset is greater than the number of elements;;");
		return 0;
		
	}
	
	//First, cache the input index;
	size_t input_index = buffer->input_index;
	
	//Then determine the index of the element to read;
	size_t index = (input_index >= offset) ? input_index - offset : buffer->length - (offset - input_index);
	
	//Get the ref of the element at the required position.
	return  CBUFFER_ELEMENT_POINTER(buffer, index);
	
}


//------------------------------------ Output ------------------------------------

/*
 * cbuffer_discard_output : discards the output element. Its integrity not guaranteed anymore;
 */

void cbuffer_discard_output(struct cbuffer *const buffer) {
	
	//If no elements are available, discarding is impossible;
	if (!buffer->nb_elements) {
		
		//Error;
		kernel_log_("circular_buffer.c : cbuffer_discard_output : the buffer is empty;");
		
		//Fail. Never reached;
		return;
		
	}
	
	//Increment safely;
	buffer->output_index = (buffer->output_index + (size_t) 1) % buffer->length;
	
	//Decrement the number of elements;
	buffer->nb_elements--;
	
	//Increment the number of spaces;
	buffer->nb_spaces++;
	
}


/*
 * Read an element from the output index, at a given offset.
 * An offset of 0 hits the output element, 1 hits the element inserted right after, ...
 */

void *cbuffer_read_output(struct cbuffer *const buffer, const size_t offset) {
	
	/*
	 * read_output is used to access previously inserted data
	 * As a consequence, the reading will be invalid if the offset is greater or equal to the number of elements
	 *  in the buffer;
	 */
	
	if (offset >= buffer->nb_elements) {
		
		//Error;
		kernel_log_("circular_buffer.c : cbuffer_read_output : offset is greater or equal"
						 " than the number of elements;");
		
		//Fail. Never reached;
		return 0;
		
	}
	
	//First, cache the output_index index;
	size_t output_index = buffer->output_index;
	
	//As an addition is required, we will switch to the larger type to avoid a type overflow;
	uint32_t larger_index = (uint32_t) output_index + (uint32_t) offset;
	
	//Then determine the index of the element to read by a modulo;
	size_t index = (uint16_t) (larger_index % (uint32_t) buffer->length);
	
	//Get the element at the required position. A modulo is applied to ensure to hit a container's element;
	return CBUFFER_ELEMENT_POINTER(buffer, index);
	
}
