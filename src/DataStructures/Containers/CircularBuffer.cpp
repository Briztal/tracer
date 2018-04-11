/*
  Queue.h - Part of TRACER

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

#ifndef TRACER_QUEUE_CPP
#define TRACER_QUEUE_CPP

#include "CircularBuffer.h"

/*
 * A macro to safely decrement an index :
 *  - if the index is zero, it takes the maximal value;
 *  - f not, it is decremented;
 */

#define SAFE_DECR(i)\
    if (i) {\
        (i)--;\
    } else {\
        (i) = max_index;\
    }\


//----------------------------------------- Constructor, destructor -----------------------------------------

/*
 * Constructor : initialises all data
 */

template<typename T>
CircularBuffer<T>::CircularBuffer(uint8_t size) :
        size(size), max_index((const uint8_t) (size - 1)), data(new buffer_element_t[size]), nb_spaces(size) {}

/*
 * Destructor : deletes the data array;
 */

template<typename T>
CircularBuffer<T>::~CircularBuffer() {
    delete[] data;
}


//---------------------------------------------Read Object Manipulation---------------------------------------------


/*
 * get_output_ptr : this method returns a pointer to the output element.
 */

template<typename T>
T *CircularBuffer<T>::get_output_ptr() {

    //First, cache the address of the queue object at the reading index;
    buffer_element_t *reading_object = data + reading_index;

    //Update the success flag;
    if (!reading_object->allocated) {
        return nullptr;//TODO USAGE FAULT;
    }

    return &reading_object->object;

}


/*
* read_previous_input : this method returns a pointer to the case that was the input case [i] inputs ago;
*/

/*

template<typename T>
T *CircularBuffer<T>::read_previous_input(uint8_t i, bool *success) {

    //First, cache the new index.
    uint8_t new_index = insertion_index + i;

    //As the offset is unknown, the first part is gonna be to convert the index into one that fits in the buffer.

    //As the index must necessarily be decreased, while it is superior to the size, decrease it of the size.
    while (new_index >= size) {

        //Decrease;
        new_index -= size;

    }

    //Now, cache the address of the queue object at the computed index;
    buffer_element_t *reading_object = data + new_index;

    //Update the success flag;
    *success = reading_object->allocated;

    //Return an actual object only if the object is allocated:
    if (*success) {
        return &reading_object->object;
    }

    //If the object was not allocated, fail.
    return nullptr;

}
*/

/*
 * discard_output : this method discards the current reading element
 */

template<typename T>
void CircularBuffer<T>::discard_output() {

    //First, cache the address of the queue object at the reading index.
    buffer_element_t *reading_object = data + reading_index;

    //Update the success flag;
    if (!reading_object->allocated)
        return;//TODO USAGE FAULT;

    //Mark the object unallocated;
    reading_object->allocated = false;

    //update the reading index
    SAFE_DECR(reading_index);

    //Modify the state variables
    nb_objects--;
    nb_spaces++;

}

template<typename T>
T CircularBuffer<T>::get_and_discard_output() {

    //First, cache the address of the queue object at the reading index.
    buffer_element_t *reading_object = data + reading_index;

    //Update the success flag;
    if (!reading_object->allocated)
        return T();//TODO USAGE FAULT;

    //Mark the object unallocated;
    reading_object->allocated = false;

    //update the reading index
    SAFE_DECR(reading_index);

    //Modify the state variables
    nb_objects--;
    nb_spaces++;

    //Return a copy of the discarded object;
    return reading_object->object;

}


//------------------------------------------Insertion Object Manipulation-------------------------------------------


/*
 * insert : this method updates the input index. The input case must have been filled before.
 */

template<typename T>
T *CircularBuffer<T>::insert() {

    //First, cache the address of the queue object at the input index.
    buffer_element_t *insertion_object = data + insertion_index;


    //If the element is already alocated, fail;
    if (insertion_object->allocated) {
        return nullptr;//TODO USAGE FAULT;
    }

    //update the reading index
    SAFE_DECR(insertion_index);

    //Modify the state variables
    nb_objects++;
    nb_spaces--;

    //Mark the object allocated;
    insertion_object->allocated = true;

    //Return the address of the object;
    return &insertion_object->object;

}


/*
 * insert_object : this method copies the element provided object to the current input case,
 *      and updates the input index.
 */

template<typename T>
void CircularBuffer<T>::insert_object(T &object) {

    //Insert an empty element and copy the object at the inserted position;
    *insert() = object;

}


//-------------------------------------------------Queue state------------------------------------------------------


template<typename T>
uint8_t CircularBuffer<T>::available_spaces() {
    return nb_spaces;
}

template<typename T>
uint8_t CircularBuffer<T>::available_elements() {
    return nb_objects;
}


//-------------------------------------------------- Clear ---------------------------------------------------------

template<typename T>
void CircularBuffer<T>::clear() {

    //Reset indices;
    reading_index = insertion_index = 0;

    //Reset the number of objects contained in the queue to zero;
    nb_objects = 0;

    //Reset the number of available spaces to the maximum number;
    nb_spaces = size;

    //Deallocate all objects :

    //For each element :
    for (uint8_t i = 0; i < size; i++) {

        //Deallocate the element;
        data[i].allocated = false;

    }


}

#undef SAFE_DECR


#endif //TRACER_QUEUE_CPP
