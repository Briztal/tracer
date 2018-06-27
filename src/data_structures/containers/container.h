/*
  container.h - Part of TRACER

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


#ifndef TRACER_CONTAINER_H
#define TRACER_CONTAINER_H

#include <stdint.h>

#include <stdbool.h>

#include <stddef.h>

typedef struct {

    //The size of an element in the array;
    const uint8_t element_size;

    //The current number of elements in the array;
    size_t nb_elements;
    
    //The maximum number of elements in the array;
    size_t max_nb_elements;

    //The elements array;
    void *elements;

} container_t;


//A macro to help us safely initialising our containers;
#define EMPTY_CONTAINER(type, max_nb) {.element_size = sizeof(type), .nb_elements = 0, .max_nb_elements = 0, .elements = 0}

#define CONTAINER_GET_TYPED(type, container_p, index) *(type *)container_get_element(container_p, index)

//Move data from one container to another;
void container_move(container_t *dst, container_t *src);

//Verify that an element can be inserted;
inline bool container_insertion_available(container_t *container) {
    return container->nb_elements < container->max_nb_elements;
}

//Insert an element at the given index;
void container_insert_element(container_t *container, size_t index, const void *element_p);

//Insert an element at the end of the array;
void container_append_element(container_t *container, const void *element_p);


//Get the element at the given index;
void *container_get_element(const container_t *container, size_t index);

//Set the element at the given index;
void container_set_element(container_t *container, size_t index, const void *element_p);


//Remove an element;
void container_remove_element(container_t *container, size_t index);

//Remove the last element of the array;
void container_remove_last_element(container_t *container);


//Delete the whole container;
void container_clear(container_t *container);

//Resize the container;
void container_resize(container_t *container, size_t new_size);


#endif //TRACER_CONTAINER_H
