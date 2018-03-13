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
 * The ValueContainer template class : a Vector class, storing objects by value;
 *
 * Elements are stored by value, contiguously.
 *
 * For storage by pointer, please check the ValueContainer Class
 */

#ifndef TRACER_TVECTOR_H
#define TRACER_TVECTOR_H

#include "stdint.h"

template<typename T>

class ValueContainer {

    //-------------------------------------- Initialisation --------------------------------------

public:

    //Constructor;
    explicit ValueContainer(uint8_t max_size);

    //Destructor;
    virtual ~ValueContainer();


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



#include <malloc.h>

#include <Interaction/Interaction.h>


//--------------------------------------------------- Initialisation ---------------------------------------------------

/*
 * Constructor : initialises the tasks array;
 */

template<class T>
ValueContainer<T>::ValueContainer(uint8_t max_size) : size(0), max_size(max_size), elements(nullptr) {}


/*
 * Destructor : frees the elements array;
 */

template<class T>
ValueContainer<T>::~ValueContainer() {

    clear();

}


//------------------------------------------------------ Builders ------------------------------------------------------


/*
 * addTask : add a task to the task list.
 *
 * This function starts to realloc the task array, and if it succeeds, saves the new task;
 */

template<class T>
bool ValueContainer<T>::add(T new_element) {


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
T ValueContainer<T>::remove(uint8_t index) {

    //If the index is invalid, stop here;
    if (index >= size)
        return T();

    //Cache the current size
    const uint8_t size = this->size;

    //Cache the appropriate element;
    T element = elements[index];

    //For every task after index;
    for (uint8_t shift_index = index + (uint8_t) 1; shift_index < size; index++, shift_index++) {

        //Shift the task of one position;
        elements[index] = elements[shift_index];

    }

    //Cache the new size
    uint8_t new_size = size - (uint8_t) 1;

    //Clear the last element to avoid deletion;
    elements[new_size] = T();

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
void ValueContainer<T>::clear() {


    //First, realloc tasks_array to zero-size;
    void *new_ptr = realloc(elements, 0);

    //If the reallocation completed :
    if (new_ptr) {

        //Update the task pointer;
        elements = (T*) new_ptr;

        //Update the size;
        size = 0;

    }
}


/*
 * isElementPresent : this function return true if the given task is already present in the queue;
 *
 *  If it finds the task, it saves its index in the given variable;
 */

template<class T>
bool ValueContainer<T>::isElementPresent(T old_task, uint8_t *index) {

    //Search for the task in the array;
    for (uint8_t task_index = 0; task_index < size; task_index++) {

        //If a matching task is found
        if (elements[task_index] == old_task) {

            //Update the index;
            *index = task_index;

            //Complete;
            return true;

        }

    }

    //Fail;
    return false;

}


//------------------------------------------------- Getters - Setters --------------------------------------------------


/*
 * getSize : this function returns the size of the list;
 */

template<class T>
uint8_t ValueContainer<T>::getSize() {

    //Return the size lol;
    return size;

}


/*
 * get_task : this function returns the task at the given index, if it is valid;
 */

template<class T>
T ValueContainer<T>::getElement(uint8_t index) {

    //If the index is invalid
    if (index >= size)
        return T();

    //If the index is valid, return the appropriate task
    return elements[index];

}


/*
 * get_task : this function returns the task at the given index, if it is valid;
 */

template<class T>
T ValueContainer<T>::setElement(uint8_t index, T new_element) {

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
 */

template<class T>
bool ValueContainer<T>::resize(uint8_t new_size) {

    //If the maximum number of tasks is reached :
    if (new_size >= max_size) {

        //Log;
        std_out("Error in ValueContainer::resize : the requested size is superior to the maximum size;");

        //Fail;
        return false;

    }

    //Then, reallocate the task array, to contain the required amount of elements;
    void *new_array = realloc(elements, new_size);

    //If the reallocation failed :
    if (!new_array && new_size) {

        //Log;
        std_out("Error in ValueContainer::resize : the reallocation failed;");

        //Fail;
        return false;

    }


    //If the reallocation completed :

    //Update the tasks array;
    elements = (T*) new_array;


    //Update the size;
    size = new_size;

    //Complete;
    return true;


}

#endif //TRACER_EVENTelementLIST_H
