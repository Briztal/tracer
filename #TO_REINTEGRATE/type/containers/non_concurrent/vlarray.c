/*
  vlarray.c - Part of TRACER

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

#include <string.h>

#include <kernel/syscall.h>

#include "vlarray.h"


/*
 * vlarray_move : moves data from one vlarray to another;
 */

void vlarray_move(vlarray_t *const dst, vlarray_t *const src) {

	//Cache dst;
	const vlarray_t tmp = *dst;

	//Copy src into dst;
	memcpy(dst, src, sizeof(vlarray_t));

	//Copy the cached dst into src;
	memcpy(src, &tmp, sizeof(vlarray_t));

}


/*
 *
 * vlarray_insert_element : starts by reallocating the vlarray array,
 *  then shifts all bytes to leave space for the new element;
 *
 *  Finally, it copies the referenced element;
 */

void vlarray_insert_element(vlarray_t *const vlarray, const size_t index, const void *const element) {

	//If the index is invalid, fail;
	if (index > vlarray->length)
		//TODO ERROR
		return;

	//Resize the array;
	vlarray_resize(vlarray, vlarray->length + (size_t) 1);

	//Cache the number of elements;
	const size_t nb_elements = vlarray->length;

	//Cache the size of an element;
	const uint8_t element_size = vlarray->element_size;


	//As we shift to the right, We must shift from the end to the initial;

	//Cache the destination;
	uint8_t *dst_ptr = vlarray->elements + (nb_elements - (uint8_t) 1);

	//Cache the source, lower of one element;
	uint8_t *src_ptr = dst_ptr - element_size;

	//All value has been reallocated, now we must shift value from the insertion index;
	for (size_t shift_counts = nb_elements - (index + (size_t) 1); shift_counts--;) {

		//Copy the value from source to destination, and decrease both pointers;
		*(dst_ptr--) = *(src_ptr--);

	}

	//The destination pointer now points at the last byte available for the element;

	//We will copy the referenced element, from the last byte to the first;
	src_ptr = (uint8_t *) element + element_size;

	//Now, copy the element;
	for (uint8_t copy_index = element_size; copy_index--;) {

		//Copy the value from source to destination, and decrease both pointers;
		*(dst_ptr--) = *(src_ptr--);

	}


}

/*
 * vlarray_append_element : insert the element at the end of the array;
 */

void vlarray_append_element(vlarray_t *const vlarray, const void *const element) {

	//Insert the element at the end of the array;
	vlarray_insert_element(vlarray, vlarray->length, element);

}


/*
 * vlarray_get_element : returns a pointer to the element at the given index;
 */

void *vlarray_get_element(const vlarray_t *const vlarray, const size_t index) {

	//If the index is invalid :
	if (index >= vlarray->length) {

		//Kernel debug;TODO KERNEL PANIC
		return 0;

	}

	//Determine the adequate pointer to return;
	return vlarray->elements + (uint32_t) (index * (size_t) vlarray->element_size);

}


/*
 * vlarray_set_element : sets the element at the given index;
 */

void vlarray_set_element(vlarray_t *const vlarray, const size_t index, const void *const element_p) {

	//If the index is invalid :
	if (index >= vlarray->length) {

		//Kernel debug;TODO KERNEL PANIC
		return;

	}

	//Cache the size of an element_p;
	size_t element_size = vlarray->element_size;

	//Cache the dest pointer;
	uint8_t *dst = ((uint8_t *)vlarray->elements) + (size_t) (index * element_size);

	//Copy the content of the element at the correct index;
	memcpy(dst, element_p, element_size);

}


//Remove an element;
void vlarray_remove_element(vlarray_t *const vlarray, const size_t index) {

	//First, cache the current size of the array;
	size_t current_size = vlarray->length;

	//If the index is invalid :
	if (index >= vlarray->length) {

		//Kernel debug;TODO KERNEL PANIC
		return;

	}


	/*
	 * We will shift all elements after the element to delete of one case;
	 */

	//Cache the element size;
	const size_t element_size = vlarray->element_size;

	//Determine source and destination pointers : addresses of (resp) the element to delete and the next one;
	uint8_t *dst_ptr = (uint8_t *) (vlarray->elements + (index * (size_t) element_size));
	const uint8_t *src_ptr = dst_ptr + element_size;

	//Determine the number of bytes to shift;
	uint32_t shift_counter = element_size * (vlarray->length - index - (size_t) 1);

	//For each byte to shift :
	for (; shift_counter--;) {

		//Copy the src byte in the destination position and block_spacing both pointers;
		(*dst_ptr++) = (*src_ptr++);

	}

	//Now, we can decrease our size of 1;
	vlarray_resize(vlarray, current_size - (size_t) 1);

}


/*
 * vlarray_remove_last_element : removes the last element of the array;
 */

void vlarray_remove_last_element(vlarray_t *const vlarray) {

	//Determine the last index, and call remove on it;
	vlarray_remove_element(vlarray, vlarray->length - (size_t) 1);

}


/*
 * vlarray_clear : resize to null pointer;
 */

void vlarray_clear(vlarray_t *const vlarray) {

	//Call resize with size-zero to delete all elements;
	vlarray_resize(vlarray, 0);

}


/*
 * vlarray_resize : this function will resize the vlarray to the required size;
 *
 *  The size is expressed in number of elements;
 */

void vlarray_resize(vlarray_t *const vlarray, const size_t new_length) {

	//If the new length is superior to the max size :
	if (new_length > vlarray->max_length) {
		return;//TODO ERROR
	}

	//Cache the element size;
	const size_t element_size = vlarray->element_size;

	//Determine the required size of the array;
	const size_t new_size = (uint32_t) new_length * (uint32_t) element_size;

	//Try to reallocate the array;
	void *new_array = kernel_realloc(vlarray->elements, new_size);

	//If the size was not zero, and the resulting pointer is null (reallocation failure);
	if (new_size && !new_array) {

		//Kernel debug; TODO ERROR;
		return;

	}

	//Update the array;
	vlarray->elements = new_array;

	//Update the size;
	vlarray->length = new_length;

}