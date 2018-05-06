//
// Created by root on 5/6/18.
//

#include <stdbool.h>
#include "circular_buffer.h"
#include "container.h"


/*
 * discard_all : discards all elements in the buffer, but doesn't change the container's size;
 */

void discard_all(cbuffer_t *cbuffer) {

    //Set the number of spaces at its maximum;
    cbuffer->nb_spaces = cbuffer->container.nb_elements;

    //Set input and output indices at their initial value;
    cbuffer->input_index = cbuffer->output_index = 0;

}

//----------------------------------- Input -----------------------------------

/*
 * cbuffer_insert : adds an element in the buffer, at the insertion position; Integrity guaranteed;
 */

void cbuffer_insert(cbuffer_t *cbuffer, void *element_p) {

    //Spaces will be verified during cbuffer_validate_input;

    //Set the element at the correct position;
    container_set_element(&cbuffer->container, cbuffer->input_index, element_p);

    //Validate the input position;
    cbuffer_validate_input(cbuffer);

}

//Increase the input index, and guarantee the integrity of the input element;
void cbuffer_validate_input(cbuffer_t *cbuffer) {

    //If no spaces are available, validation is impossible;
    if (!cbuffer->nb_spaces)
        return;//TODO ERROR;


    //Increment safely;
    cbuffer->input_index = (cbuffer->input_index + (container_index_t) 1) % cbuffer->container.nb_elements;

}


/*
 * cbuffer_read_input : read an element from the input point, at a given offset.
 *  An offset of 0 hits the input element, 1 hits the element inserted just before, etc...
 */

inline void *cbuffer_read_input(cbuffer_t *cbuffer, container_index_t offset) {

    /*
     * read_input is used to access previously inserted data, but also to update the element we will validate;
     * As a consequence, the reading will be invalid if the offset is strictly greater than the number of elements
     *  in the buffer;
     */

    if (offset > (cbuffer->container.nb_elements - cbuffer->nb_spaces)) {
        return 0; //TODO ERROR;
    }

    //First, cache the input index;
    container_index_t input_index = cbuffer->input_index;

    //Then determine the index of the element to read;
    container_index_t index = (input_index >= offset) ? input_index - offset : cbuffer->container.nb_elements -
                                                                               (offset - input_index);

    //Get the element at the required position. A modulo is applied to ensure to hit a container's element;
    return container_get_element(&cbuffer->container, index);

}


//------------------------------------ Output ------------------------------------

//Discard the output element. Integrity not guaranteed anymore;
void cbuffer_discard_output(cbuffer_t *cbuffer) {

    //If no elements are available (nb of spaces equal to the max nb of elements), discarding is impossible;
    if (!cbuffer->nb_spaces == cbuffer->container.nb_elements)
        return;//TODO ERROR;

    //Increment safely;
    cbuffer->output_index = (cbuffer->output_index + (container_index_t) 1) % cbuffer->container.nb_elements;

}


/*
 * Read an element from the output index, at a given offset.
 * An offset of 0 hits the output element, 1 hits the element inserted right after, ...
 */

void *cbuffer_read_output(cbuffer_t *cbuffer, container_index_t offset) {

    //TODO ERROR IF BOUNDS OVERRUN

    /*
     * read_output is used to access previously inserted data
     * As a consequence, the reading will be invalid if the offset is greater or equal to the number of elements
     *  in the buffer;
     */

    if (offset >= (cbuffer->container.nb_elements - cbuffer->nb_spaces)) {
        return 0; //TODO ERROR;
    }

    //First, cache the output_index index;
    container_index_t output_index = cbuffer->output_index;

    //As an addition is required, we will switch to the larger type to avoid a type overflow;
    uint32_t larger_index = (uint32_t) output_index + (uint32_t) offset;

    //Then determine the index of the element to read by a modulo;
    container_index_t index = (uint16_t) (larger_index % (uint32_t) cbuffer->container.nb_elements);

    //Get the element at the required position. A modulo is applied to ensure to hit a container's element;
    return container_get_element(&cbuffer->container, index);

}


//------------------------------------ Resize ------------------------------------

/*
 * cbuffer_resize : resizes the circular buffer.
 *  All elements pointers that have been queried are invalidated;
 */

void cbuffer_resize(cbuffer_t *cbuffer, container_index_t new_size) {

    //Dynamic resize is not supported for instance;
    if (cbuffer->nb_spaces != cbuffer->container.nb_elements) {

        return;//TODO ERROR;

    }

    //Resize the container;
    container_resize(&cbuffer->container, new_size);

    //Reset its data;
    cbuffer->nb_spaces = 0;
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
    if (new_size <= cbuffer->container.nb_elements) {
        return;//TODO ERROR;
    }

    bool rearrangement = (cbuffer->input_index < cbuffer->output_index) || cbuffer->nb_spaces;

    //Cache the old size of the container;
    container_index_t old_size =  cbuffer->container.nb_elements;

    //Cache the number of added elements;
    uint8_t added_elements =

    //Increment the number of spaces of the number of added cases;
    cbuffer->nb_spaces += new_size - cbuffer->container.nb_elements;

    //Then, resize the array;
    container_resize(&cbuffer->container, new_size);

    //If rearrangement is required :
    if (rearrangement) {

        */

    }


}
