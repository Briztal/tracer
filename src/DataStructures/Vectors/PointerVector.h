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
 * The PointerVector template class : a Vector class, storing pointers to objects;
 *
 * Elements must be pointers to objects, as the class handles itself deletion of unused elements;
 *
 * For storage by value, please check the ValueVector Class
 */

#ifndef TRACER_POINTERVECTOR_H
#define TRACER_POINTERVECTOR_H

#include "stdint.h"

template<typename T>
class PointerVector {

    //-------------------------------------- Initialisation --------------------------------------

public:

    //Constructor;
    explicit PointerVector(uint8_t max_size);

    //Destructor;
    virtual ~PointerVector();


    //-------------------------------------- Builders --------------------------------------

public:

    //Add a new element to the list;
    bool add(T *new_element);

    //Remove a element from the list if it is present;
    T *remove(uint8_t index);

    //Remove all element;
    void clear();


    //-------------------------------------- Getters --------------------------------------

public:

    //Get the number of elements in the list;
    uint8_t getSize();

    //Get a particular element;
    T *getElement(uint8_t index);

    //Set a particular element;
    T *setElement(uint8_t index, T *new_element);


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
    T **elements;

};





#include <malloc.h>

#include <Interaction/Interaction.h>

#include "PointerVector.h"


//--------------------------------------------------- Initialisation ---------------------------------------------------

/*
 * Constructor : initialises the tasks array;
 */

template<class T>
PointerVector<T>::PointerVector(uint8_t max_size) : size(0), max_size(max_size), elements(nullptr) {}


/*
 * Destructor : deletes every element and frees the dynamic tasks array;
 */

template<class T>
PointerVector<T>::~PointerVector() {

    clear();

}


//------------------------------------------------------ Builders ------------------------------------------------------


/*
 * addTask : add a task to the task list.
 *
 * This function starts to realloc the task array, and if it succeeds, saves the new task;
 */

template<class T>
bool PointerVector<T>::add(T *new_element) {


    //Increment the size;
    uint8_t old_size = size;

    //If the resizing completed
    if (resize(old_size + (uint8_t) 1)) {

        //Save the last task;
        elements[old_size] = new_element;

        //Complete;
        return true;

    }

    //Fail if the reallocation failed;
    return false;

}


/*
 * remove : this function will search if the provided element exists in the array, and if so, it will
 *  remove it.
 *
 *  After removing the task, it will reallocate the task array to save the space;
 */

template<class T>
T *PointerVector<T>::remove(uint8_t index) {

    //If the index is invalid, stop here;
    if (index >= size)
        return nullptr;

    //Cache the current size
    const uint8_t size = this->size;

    //Cache the appropriate element;
    T *element = elements[index];

    //For every task after index;
    for (uint8_t shift_index = index + (uint8_t) 1; shift_index < size; index++, shift_index++) {

        //Shift the task of one position;
        elements[index] = elements[shift_index];

    }

    //Cache the new size
    uint8_t new_size = size - (uint8_t) 1;

    //Clear the last element to avoid deletion;
    elements[new_size] = nullptr;

    //If the reallocation failed
    if (!resize(new_size)) {

        //Log;
        std_out("Error in TasksVector::addTask : the reallocation failed;");

    }

    //Return the deleted element;
    return element;

}


/*
 * clear : this function clears the task array;
 */

template<class T>
void PointerVector<T>::clear() {

    //First, delete each element;
    for (uint8_t element_index = 0; element_index < size; element_index++) {

        //Delete the element;
        delete elements[element_index];

    }

    //First, realloc tasks_array to zero-size;
    void *new_ptr = realloc(elements, 0);

    //If the reallocation completed :
    if (new_ptr) {

        //Update the task pointer;
        elements = (T**) new_ptr;

        //Update the size;
        size = 0;

    }
}


//------------------------------------------------- Getters - Setters --------------------------------------------------


/*
 * getSize : this function returns the size of the list;
 */

template<class T>
uint8_t PointerVector<T>::getSize() {

    //Return the size lol;
    return size;

}


/*
 * get_task : this function returns the task at the given index, if it is valid;
 */

template<class T>
T *PointerVector<T>::getElement(uint8_t index) {

    //If the index is invalid
    if (index >= size)
        return nullptr;

    //If the index is valid, return the appropriate task
    return elements[index];

}


/*
 * get_task : this function returns the task at the given index, if it is valid;
 */

template<class T>
T *PointerVector<T>::setElement(uint8_t index, T *new_element) {

    //If the index is invalid
    if (index >= size)
        return nullptr;

    //If the index is valid, cache the old element;
    T *old_element = elements[index];

    //Update the required element;
    elements[index] = new_element;

    //Return the old element for further delete;
    return old_element;

}


/*
 * resize : this function will attempt to resize the array to the provided size;
 *
 * It will automatically delete removed elements, and initialise added elements to nullptr;
 */

template<class T>
bool PointerVector<T>::resize(uint8_t new_size) {

    //If the maximum number of tasks is reached :
    if (new_size >= max_size) {

        //Log;
        std_out("Error in PointerVector::resize : the requested size is superior to the maximum size;");

        //Fail;
        return false;

    }

    //First, we must delete objects comprised in [|new_size, size|[.

    //For every element :
    for (uint8_t element_index = new_size; element_index < size; element_index++) {

        //Delete the element;
        delete elements[element_index];

    }

    //Then, reallocate the task array, to contain one more element;
    void *new_array = realloc(elements, new_size);

    //If the reallocation failed :
    if (!new_array && new_size) {

        //Log;
        std_out("Error in PointerVector::resize : the reallocation failed;");

        //Fail;
        return false;

    }


    //If the reallocation completed :

    //Update the tasks array;
    elements = (T**) new_array;


    //Finally, we must initialise pointers comprised in [|size, new_size|[ to nullptr;

    //For every element :
    for (uint8_t element_index = size; element_index < new_size; element_index++) {

        //Initialise the element to nullptr;
        elements[element_index] = nullptr;

    }

    //Update the size;
    size = new_size;

    //Complete;
    return true;


}

#endif