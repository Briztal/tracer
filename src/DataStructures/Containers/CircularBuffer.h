/*
  CircularBuffer.h - Part of TRACER

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

#ifndef TRACER_CircularBuffer_H
#define TRACER_CircularBuffer_H

#include <stdint.h>

template<typename T>
class CircularBuffer {

private:

/*
 * This struct will be the basic object we will store in the CircularBuffer.
 */
    struct buffer_element_t {

        //The effective object we want to store.
        T object;

        //A flag, to notify whether the object has been allocated.
        bool allocated = false;

    };
    

    //----------------------------------------- Constructor, destructor -----------------------------------------

public:

    //Constructor;
    explicit CircularBuffer(uint8_t size);

     //Destructor : deletes the data array;
    ~CircularBuffer();


    //---------------------------------------------Read Object Manipulation---------------------------------------------

public:

    //Returns a pointer to the output element.
    T *get_output_ptr();

    //Discards the current reading element
    void discard_output();

    //Get a copy of the output element and discard it;
    T get_and_discard_output();

    //Returns a pointer to the case that was the input case [i] inputs ago.
    //T *read_previous_input(uint8_t i, bool *success);


    //------------------------------------------Insertion Object Manipulation-------------------------------------------

    //Updates the input index and return a pointer to the inserted element.
    T* insert();

    //Copies the element provided object to the current input case;
    void insert_object(T &object);


    //-------------------------------------------------CircularBuffer state------------------------------------------------------

    //Get the number of available spaces;
    uint8_t available_spaces();

    //Get the number of available elements;
    uint8_t available_elements();


    //-------------------------------------------------- Clear ---------------------------------------------------------

    //Reset all flags and indices to initial position;
    void clear();


    //----------------------------------------------- CircularBuffer fields -----------------------------------------------------

private:

    //The size of the CircularBuffer;
    const uint8_t size;

    //The maximum index (size - 1);
    const uint8_t max_index;

    //The array of CircularBuffer objects;
    buffer_element_t *const data;

    //The reading and insertion indices, initialised at zero;
    uint8_t reading_index = 0;
    uint8_t insertion_index = 0;

    //The number of objects and spaces in the CircularBuffer;
    uint8_t nb_objects = 0;
    uint8_t nb_spaces;

};


#endif //TRACER_CircularBuffer_H
