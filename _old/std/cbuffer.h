/*
  circular_buffer.h - Part of TRACER

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

#ifndef TRACER_CIRCULAR_BUFFER_H
#define TRACER_CIRCULAR_BUFFER_H


#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

struct cbuffer {
	
	//The array we own;
	void *array;
	
	//The max number of elements in the array we own;
	const size_t length;
	
	//The byte size of an element;
	const size_t element_size;
	
	//The read index;
	size_t input_index;
	
	//The write index;
	size_t output_index;
	
	//The number of spaces available in the array;
	size_t nb_spaces;
	
	//The number of elements available in the array;
	size_t nb_elements;
	
};


//------------------------------------ Init - clear ------------------------------------

//A macro to help us safely initialising our buffer;
#define CBUFFER(_array, _length, elmt_size) {\
	.array = (_array), .length = (_length), .element_size = (elmt_size),\
	.input_index = 0, .output_index = 0,\
	.nb_spaces = 0, .nb_spaces = (_length),\
}


//Discard all elements in the buffer. The container's size remains untouched;
void cbuffer_discard_all(struct cbuffer *);


//----------------------------------- Input -----------------------------------

//Add an element in the buffer, at the insertion position; Integrity guaranteed;
void cbuffer_insert(struct cbuffer *, const void *element_p);

//Increase the input index, and guarantee the integrity of the input element;
void cbuffer_validate_input(struct cbuffer *);

/*
 * Read an element from the input point, at a given offset.
 * An offset of 0 hits the input element, 1 hits the element inserted just before, etc...
 */
void *cbuffer_read_input(const struct cbuffer *, size_t offset);


//------------------------------------ Output ------------------------------------

//Discard the output element. Integrity not guaranteed anymore;
void cbuffer_discard_output(struct cbuffer *buffer);

/*
 * Read an element from the output index, at a given offset.
 * An offset of 0 hits the output element, 1 hits the element inserted right after, ...
 */
void *cbuffer_read_output(struct cbuffer *, size_t offset);


#endif //TRACER_CIRCULAR_BUFFER_H

