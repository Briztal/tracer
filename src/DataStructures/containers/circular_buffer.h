//
// Created by root on 5/6/18.
//

#ifndef TRACER_CIRCULAR_BUFFER_H
#define TRACER_CIRCULAR_BUFFER_H

#include "container.h"

typedef struct {

    //The read index;
    container_index_t input_index;

    //The write index;
    container_index_t output_index;

    //The number of spaces available in the array;
    container_index_t nb_spaces;

    //The container data type;
    container_t container;

} cbuffer_t;


//------------------------------------ Init - clear ------------------------------------

//A macro to help us safely initialising our containers;
#define EMPTY_CBUFFER(type) {.input_index = 0, .output_index = 0, .nb_spaces = 0, .container = EMPTY_CONTAINER(type)}

//Discard all elements in the buffer. The container's size remains untouched;
void discard_all(cbuffer_t *);

//----------------------------------- Input -----------------------------------

//Add an element in the buffer, at the insertion position; Integrity guaranteed;
void cbuffer_insert(cbuffer_t *, void *element_p);

//Increase the input index, and guarantee the integrity of the input element;
void cbuffer_validate_input(cbuffer_t*);

/*
 * Read an element from the input point, at a given offset.
 * An offset of 0 hits the input element, 1 hits the element inserted just before, etc...
 */
inline void *cbuffer_read_input(cbuffer_t*, container_index_t offset);


//------------------------------------ Output ------------------------------------

//Discard the output element. Integrity not guaranteed anymore;
void cbuffer_discard_output(cbuffer_t *cbuffer);

/*
 * Read an element from the output index, at a given offset.
 * An offset of 0 hits the output element, 1 hits the element inserted right after, ...
 */
void *cbuffer_read_output(cbuffer_t*, container_index_t offset);


//------------------------------------ Resize ------------------------------------

//Resize the circular buffer. All elements pointers that have been queried are invalidated;
void cbuffer_resize(cbuffer_t*, container_index_t new_size);

#endif //TRACER_CIRCULAR_BUFFER_H
