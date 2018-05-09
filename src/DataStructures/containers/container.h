//
// Created by root on 5/5/18.
//

#ifndef TRACER_CONTAINER_H
#define TRACER_CONTAINER_H

#include <stdint.h>

typedef uint16_t container_index_t;

typedef struct {

    //The size of an element in the array;
    const uint8_t element_size;

    //The current number of elements in the array;
    container_index_t nb_elements;

    //The elements array;
    void *elements;

} container_t;


//A macro to help us safely initialising our containers;
#define EMPTY_CONTAINER(type) {.nb_elements = 0, .element_size = sizeof(type), .elements = 0}

#define CONTAINER_GET_TYPED(type, container_p, index) *(type *)container_get_element(container_p, index)

//Move data from one container to another;
void container_move(container_t *dst, container_t *src);

//Insert an element at the given index;
void container_insert_element(container_t *container, container_index_t index, void *element_p);

//Insert an element at the end of the array;
void container_append_element(container_t *container, void *element_p);


//Get the element at the given index;
void *container_get_element(container_t *container, container_index_t index);

//Set the element at the given index;
void container_set_element(container_t *container, container_index_t index, void *element_p);


//Remove an element;
void container_remove_element(container_t *container, container_index_t index);

//Remove the last element of the array;
void container_remove_last_element(container_t *container);


//Delete the whole container;
void container_clear(container_t *container);

//Resize the container;
void container_resize(container_t *container, container_index_t new_size);


#endif //TRACER_CONTAINER_H
