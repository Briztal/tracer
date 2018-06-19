/*
  container.c - Part of TRACER

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

#include <kernel/kernel.h>

#include "container.h"


/*
 * container_move : moves data from one container to another;
 */

void container_move(container_t *dst, container_t *src) {

    //Cache dst;
    const container_t tmp = *dst;

    //Copy src into dst;
    memcpy(dst, src, sizeof(container_t));

    //Copy the cached dst into src;
    memcpy(src, &tmp, sizeof(container_t));

}



/*
 *
 * container_insert_element : starts by reallocating the container array,
 *  then shifts all bytes to leave space for the new element;
 *
 *  Finally, it copies the referenced element;
 */

void container_insert_element(container_t *container, const size_t index, void *element) {

    //If the index is invalid, fail;
    if (index > container->nb_elements)
        //TODO ERROR
        return;

    //Resize the array;
    container_resize(container, container->nb_elements + (size_t) 1);

    //Cache the number of elements;
    const size_t nb_elements = container->nb_elements;

    //Cache the size of an element;
    const uint8_t element_size = container->element_size;


    //As we shift to the right, We must shift from the end to the initial;

    //Cache the destination;
    uint8_t *dst_ptr = container->elements + (nb_elements - (uint8_t) 1);

    //Cache the source, lower of one element;
    uint8_t *src_ptr = dst_ptr - element_size;

    //All value has been reallocated, now we must shift value from the insertion index;
    for (size_t shift_counts = nb_elements - (index + (size_t) 1); shift_counts--;) {

        //Copy the value from source to dest, and decrease both pointers;
        *(dst_ptr--) = *(src_ptr--);

    }

    //The destination pointer now points at the last byte available for the element;

    //We will copy the referenced element, from the last byte to the first;
    src_ptr = (uint8_t *) element + element_size;

    //Now, copy the element;
    for (uint8_t copy_index = element_size; copy_index--;) {

        //Copy the value from source to dest, and decrease both pointers;
        *(dst_ptr--) = *(src_ptr--);

    }


}

/*
 * container_append_element : insert the element at the end of the array;
 */

void container_append_element(container_t *container, void *element) {

    //Insert the element at the end of the array;
    container_insert_element(container, container->nb_elements, element);

}


/*
 * container_get_element : returns a pointer to the element at the given index;
 */

void *container_get_element(container_t *container, size_t index) {

    //If the index is invalid :
    if (index >= container->nb_elements) {

        //Kernel panic;TODO KERNEL PANIC
        return 0;

    }

    //Determine the adequate pointer to return;
    return container->elements + (uint32_t) (index * (size_t) container->element_size);

}


/*
 * container_set_element : sets the element at the given index;
 */

void container_set_element(container_t *container, size_t index, void *element) {

    //If the index is invalid :
    if (index >= container->nb_elements) {

        //Kernel panic;TODO KERNEL PANIC
        return;

    }

    //Cache the src and dst pointer;
    uint8_t *dst_ptr = container->elements + (uint32_t) (index * (size_t) container->element_size);
    uint8_t *src_ptr = element;

    //For each byte to copy;
    for (uint8_t byte_count = container->element_size; byte_count--;) {

        //Copy the src byte at the dst address;
        *(dst_ptr++) = *(src_ptr++);

    }

}


//Remove an element;
void container_remove_element(container_t *container, size_t index) {

    //First, cache the current size of the array;
    size_t current_size = container->nb_elements;

    //If the index is invalid :
    if (index >= container->nb_elements) {

        //Kernel panic;TODO KERNEL PANIC
        return;

    }


    /*
     * We will shift all elements after the element to delete of one case;
     */

    //Cache the element size;
    const uint8_t element_size = container->element_size;

    //Determine source and dest pointers : addresses of (resp) the element to delete and the next one;
    uint8_t *dst_ptr = (uint8_t *) (container->elements + (index * (size_t) element_size));
    uint8_t *src_ptr = dst_ptr + element_size;

    //Determine the number of bytes to shift;
    uint32_t shift_counter = element_size * (container->nb_elements - index - (size_t) 1);

    //For each byte to shift :
    for (; shift_counter--;) {

        //Copy the src byte in the dest position and increment both pointers;
        (*dst_ptr++) = (*src_ptr++);

    }

    //Now, we can decrease our size of 1;
    container_resize(container, current_size - (size_t) 1);

}


/*
 * container_remove_last_element : removes the last element of the array;
 */

void container_remove_last_element(container_t *container) {

    //Determine the last index, and call remove on it;
    container_remove_element(container, container->nb_elements - (size_t) 1);

}


/*
 * container_clear : resize to null pointer;
 */

void container_clear(container_t *container) {

    //Call resize with size-zero to delete all elements;
    container_resize(container, 0);

}


/*
 * container_resize : this function will resize the container to the required size;
 *
 *  The size is expressed in number of elements;
 */

void container_resize(container_t *container, const size_t new_size) {

    //If the new size is superior to the max size :
    if (new_size > container->max_nb_elements) {
        return;//TODO ERROR
    }

    //Cache the element size;
    const uint8_t element_size = container->element_size;

    //Determine the required size of the array;
    uint32_t new_array_size = (uint32_t) new_size * (uint32_t) element_size;

    //Try to reallocate the array;
    void *new_array = kernel_realloc(container->elements, new_array_size);

    //If the size was not zero, and the resulting pointer is null (reallocation failure);
    if (new_array_size && !new_array) {

        //Kernel panic; TODO ERROR;
        return;

    }

    //Update the array;
    container->elements = new_array;

    //Update the size;
    container->nb_elements = new_size;

}