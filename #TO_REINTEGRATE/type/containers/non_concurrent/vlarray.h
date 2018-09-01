/*
  vlarray.h - Part of TRACER

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


#ifndef TRACER_vlarray_H
#define TRACER_vlarray_H

#include <stdint.h>

#include <stdbool.h>

#include <stddef.h>

typedef struct {

    //The size of an element in the array;
    const size_t element_size;

    //The current number of elements in the array;
    size_t length;
    
    //The maximum number of elements in the array;
    size_t max_length;

    //The elements array;
    void *elements;

} vlarray_t;


//A macro to help us safely initialising our vlarrays;
#define EMPTY_VLARRAY(type, maximal_length) {.element_size = sizeof(type), .length = 0, .max_length = (maximal_length), .elements = 0}

#define VLARRAY_GET_TYPED(type, vlarray_p, index) *(type *)vlarray_get_element(vlarray_p, index)

//Move data from one vlarray to another;
void vlarray_move(vlarray_t *dst, vlarray_t *src);

//Verify that an element can be inserted;
inline bool vlarray_insertion_available(vlarray_t *vlarray) {
    return vlarray->length < vlarray->max_length;
}

//Insert an element at the given index;
void vlarray_insert_element(vlarray_t *vlarray, size_t index, const void *element_p);

//Insert an element at the end of the array;
void vlarray_append_element(vlarray_t *vlarray, const void *element_p);


//Get the element at the given index;
void *vlarray_get_element(const vlarray_t *vlarray, size_t index);

//Set the element at the given index;
void vlarray_set_element(vlarray_t *vlarray, size_t index, const void *element_p);


//Remove an element;
void vlarray_remove_element(vlarray_t *vlarray, size_t index);

//Remove the last element of the array;
void vlarray_remove_last_element(vlarray_t *vlarray);


//Delete the whole vlarray;
void vlarray_clear(vlarray_t *vlarray);

//Resize the vlarray;
void vlarray_resize(vlarray_t *vlarray, size_t new_length);


#endif //TRACER_vlarray_H
