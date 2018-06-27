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

#include "container.h"

typedef struct {

	//The read index;
	size_t input_index;

	//The write index;
	size_t output_index;

	//The number of spaces available in the array;
	size_t nb_spaces;

	//The number of elements available in the array;
	size_t nb_elements;

	//The container data type;
	container_t container;

} cbuffer_t;


//------------------------------------ Init - clear ------------------------------------

//A macro to help us safely initialising our containers;
#define EMPTY_CBUFFER(type, max_size) {.input_index = 0, .output_index = 0, .nb_spaces = 0, .container = EMPTY_CONTAINER(type, max_size)}

//Discard all elements in the buffer. The container's size remains untouched;
void cbuffer_discard_all(cbuffer_t *);

//----------------------------------- Input -----------------------------------

//Add an element in the buffer, at the insertion position; Integrity guaranteed;
void cbuffer_insert(cbuffer_t *, const void *element_p);

//Increase the input index, and guarantee the integrity of the input element;
void cbuffer_validate_input(cbuffer_t *);

/*
 * Read an element from the input point, at a given offset.
 * An offset of 0 hits the input element, 1 hits the element inserted just before, etc...
 */
void *cbuffer_read_input(const cbuffer_t *, size_t offset);


//------------------------------------ Output ------------------------------------

//Discard the output element. Integrity not guaranteed anymore;
void cbuffer_discard_output(cbuffer_t *cbuffer);

/*
 * Read an element from the output index, at a given offset.
 * An offset of 0 hits the output element, 1 hits the element inserted right after, ...
 */
void *cbuffer_read_output(cbuffer_t *, size_t offset);


//------------------------------------ Resize ------------------------------------

//Resize the circular buffer. All elements pointers that have been queried are invalidated;
void cbuffer_resize(cbuffer_t *, size_t new_size);

#endif //TRACER_CIRCULAR_BUFFER_H
