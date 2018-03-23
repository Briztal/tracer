/*
  PointerVector.h - Part of TRACER

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
 * The DynamicPointerBuffer template class : a Vector class, storing pointers to objects;
 *
 * Elements must be pointers to objects, as the class handles itself deletion of unused elements;
 *
 * For storage by value, please check the DynamicBuffer Class
 */

#ifndef TRACER_DYNAMICPOINTERBUFFER_H
#define TRACER_DYNAMICPOINTERBUFFER_H

#include "stdint.h"

template<typename T>
class DynamicPointerBuffer {

    //-------------------------------------- Constructor - Destructor --------------------------------------

public:

    //Constructor;
    explicit DynamicPointerBuffer(uint8_t max_size);

    //Destructor;
    virtual ~DynamicPointerBuffer();

    //---------------------------------------- Copy Constructor ----------------------------------------

    //Copy constructor;
    DynamicPointerBuffer(const DynamicPointerBuffer<T> &container);

    //Move constructor;
    DynamicPointerBuffer(DynamicPointerBuffer<T> &&container) noexcept;


    //---------------------------------------- Assignment operator ----------------------------------------

    //Copy assignment operator;
    DynamicPointerBuffer &operator=(const DynamicPointerBuffer<T> &container);

    //Copy assignment operator;
    DynamicPointerBuffer &operator=(DynamicPointerBuffer<T> &&container) noexcept;

    //Swap function;
    void swap(DynamicPointerBuffer &a, DynamicPointerBuffer<T> &b);

    //-------------------------------------- Builders --------------------------------------

public:

    //Add a new element to the list;
    bool add(T *new_element);

    //Set an element in the list,
    bool set(uint8_t index, T *new_element);

    //Remove a element from the list if it is present;
    T *remove(uint8_t index);

    //Remove all element;
    virtual void clear();


    //-------------------------------------- Concatenation --------------------------------------

    //Concatenate an lvalue array on the right;
    bool rightConcatenation(DynamicPointerBuffer<T> &&src);

    //Concatenate an lvalue array on the left;
    bool leftConcatenation(DynamicPointerBuffer<T> &&src);

    //-------------------------------------- Getters --------------------------------------

public:

    //Get the number of elements in the list;
    uint8_t getSize() const;

    //Get the number of elements in the list;
    uint8_t getMaxSize() const;

    //Get a particular element;
    T *getElement(uint8_t index) const;

    //Set a particular element;
    T *setElement(uint8_t index, T *new_element);


    //-------------------------------------- Sort --------------------------------------

public:

    //Get the number of elements in the list;
    void sort();


    //-------------------------------------- Resizing --------------------------------------

protected:

    //Attempt to resize the array to the given size. WARNING : DOES NOT DELETE REMOVED OBJECTS !
    bool resizeTo(uint8_t new_size);


    //-------------------------------------- Fields --------------------------------------


private:

    //The number of elements in the list;
    uint8_t size;

    //The maximum number of elements in the list;
    uint8_t maxSize;

    //Items to execute;
    T **elements;

};

#include "DynamicPointerBuffer.cpp"

#endif //TRACER_DYNAMICPOINTERBUFFER_H