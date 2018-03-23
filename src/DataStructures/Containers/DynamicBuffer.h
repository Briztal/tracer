/*
  ValueVector.h - Part of TRACER

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


/*
 * The DynamicBuffer template class : a Vector class, storing objects by value;
 *
 * Elements are stored by value, contiguously.
 *
 * For storage by pointer, please check the DynamicBuffer Class
 */

#ifndef TRACER_DYNAMICBUFFER_H
#define TRACER_DYNAMICBUFFER_H

#include "stdint.h"

template<typename T>

class DynamicBuffer {

    //-------------------------------------- Initialisation --------------------------------------

public:

    //Constructor;
    explicit DynamicBuffer(uint8_t max_size);

    //Destructor;
    virtual ~DynamicBuffer();

    //---------------------------------------- Copy Constructor ----------------------------------------

    //Copy constructor;
    DynamicBuffer(const DynamicBuffer &container);

    //Move constructor;
    DynamicBuffer(DynamicBuffer &&container) noexcept;


    //---------------------------------------- Assignment operator ----------------------------------------

    //Copy assignment operator;
    DynamicBuffer &operator=(const DynamicBuffer &container);

    //Copy assignment operator;
    DynamicBuffer &operator=(DynamicBuffer &&container) noexcept;

    //Swap function;
    void swap(DynamicBuffer &a, DynamicBuffer &b);


    //-------------------------------------- Builders --------------------------------------

public:

    virtual //Add a new element to the list;
    bool add(T new_element);

    //Remove a element from the list if it is present;
    T remove(uint8_t index);

    //Remove all element;
    void clear();

protected:

    //Verify that a element is present in the array and eventually save its index;
    bool isElementPresent(T old_element, uint8_t *index);


    //-------------------------------------- Getters --------------------------------------


public:

    //Get the number of elements in the list;
    uint8_t getSize();

    //Get a particular element;
    T getElement(uint8_t index);

    //Set a particular element;
    T setElement(uint8_t index, T new_element);


    //-------------------------------------- Resizing --------------------------------------

private:

    //Attempt to resize the array to the given size. WARNING : DOES NOT DELETE REMOVED OBJECTS !
    bool resize(uint8_t new_size);


    //-------------------------------------- Fields --------------------------------------

private:

    //The number of elements in the list;
    uint8_t size;

    //The maximum number of elements in the list;
    uint8_t max_size;

    //Items to execute;
    T *elements;

};

#include "DynamicBuffer.cpp"

#endif //TRACER_DYNAMICBUFFER_H
