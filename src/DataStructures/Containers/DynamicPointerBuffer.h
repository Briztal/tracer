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

#ifndef TRACER_POINTERCONTAINER_H
#define TRACER_POINTERCONTAINER_H

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


#include <malloc.h>


//--------------------------------------------------- Initialisation ---------------------------------------------------

/*
 * Constructor : initialises the tasks array;
 */

template<class T>
DynamicPointerBuffer<T>::DynamicPointerBuffer(uint8_t max_size) : size(0), maxSize(max_size), elements(nullptr) {}


/*
 * Destructor : deletes every element and frees the dynamic tasks array;
 */

template<class T>
DynamicPointerBuffer<T>::~DynamicPointerBuffer() {

    clear();

}


//---------------------------------------- Copy Constructor ----------------------------------------

/*
 * Copy Constructor : recreates the array;
 */

template<typename T>
DynamicPointerBuffer<T>::DynamicPointerBuffer(const DynamicPointerBuffer &container) : DynamicPointerBuffer(container.maxSize) {

    //First, we will update sizes;
    resizeTo(container.size);

    //Copy every element;
    for (uint8_t element_index = size; element_index--;) {

        //Insert a copy of the element at the current index;
        elements[element_index] = new T(*container.elements[element_index]);

    }

}


/*
 * Move constructor : moves src's data to us and resets its;
 */

template<typename T>
DynamicPointerBuffer<T>::DynamicPointerBuffer(DynamicPointerBuffer &&container) : size(container.size),
                                                                      maxSize(container.maxSize),
                                                                      elements(container.elements) {

    //Reset the source container's data;
    container.size = 0;
    container.elements = nullptr;

}

//---------------------------------------- Assignment operator ----------------------------------------

/*
 * Copy assignment operator : checks the value of the pointer, and if required, creates a copy and swaps;
 */

template<typename T>
DynamicPointerBuffer<T> &DynamicPointerBuffer<T>::operator=(const DynamicPointerBuffer<T> &container) {

    //If the container is us, do nothing;
    if (&container == this) {
        return *this;
    }

    //Create a copy;
    DynamicPointerBuffer copy = DynamicPointerBuffer(container);

    //Swap, to that our data is deleted at the end of copy's scope;
    swap(*this, copy);

    //Return a reference to us;
    return *this;

}


/*
 * Move assignment operator : swaps;
 */

template<typename T>
DynamicPointerBuffer<T> &DynamicPointerBuffer<T>::operator=(DynamicPointerBuffer<T> &&container) noexcept {

    //Swap, to that our data is deleted at the end of container's scope;
    swap(*this, container);

    //Return a reference to us;
    return *this;

}


/*
 * Swap : this function will swap content of the two containers;
 */

template<typename T>
void DynamicPointerBuffer<T>::swap(DynamicPointerBuffer<T> &a, DynamicPointerBuffer<T> &b) {

    //Swap max sizes;
    uint8_t ts = a.maxSize;
    a.maxSize = b.maxSize;
    b.maxSize = ts;

    //Swap sizes;
    ts = a.size;
    a.size = b.size;
    b.size = ts;

    //Swap contents;
    T **ptr = a.elements;
    a.elements = b.elements;
    b.elements = ptr;

}

//------------------------------------------------------ Builders ------------------------------------------------------


/*
 * add : add an object pointer to the elelemnt list.
 *
 * This function starts to realloc the task array, and if it succeeds, saves the new task;
 */


#include "Arduino.h"

template<class T>
bool DynamicPointerBuffer<T>::add(T *new_element) {

    //Increment the size;
    uint8_t old_size = size;

    //If the resizing completed
    if (resizeTo(old_size + (uint8_t) 1)) {

        //Save the last task;
        elements[old_size] = new_element;

        //Complete;
        return true;

    }

    //Fail if the reallocation failed;
    return false;

}


/*
 * set : sets the required element to the given pointer;
 */

template<class T>
bool DynamicPointerBuffer<T>::set(uint8_t index, T *new_element) {


    //Increment the size;
    uint8_t old_size = size;

    //If the resizing completed
    if (resizeTo(old_size + (uint8_t) 1)) {

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
T *DynamicPointerBuffer<T>::remove(uint8_t index) {

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
    if (!resizeTo(new_size)) {

        //Log;
        //std_out("Error in TasksVector::addTask : the reallocation failed;");

    }

    //Return the deleted element;
    return element;

}


/*
 * clear : this function clears the task array;
 */

template<class T>
void DynamicPointerBuffer<T>::clear() {

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
        elements = (T **) new_ptr;

        //Update the size;
        size = 0;

    }
}

//-------------------------------------- Concatenation --------------------------------------

/*
 * rightConcatenation : concatenates the given rvalue container on the right;
 */

template<typename T>
bool DynamicPointerBuffer<T>::rightConcatenation(DynamicPointerBuffer<T> &&src) {

    //Cache current sizes;
    uint8_t size = this->size;
    uint8_t src_size = src.size;

    //First, determine the final size;
    uint16_t final_size = (uint16_t) size + (uint16_t) src.getSize();

    //If the concatenation can't be done : fail;
    if (final_size > (uint16_t) getMaxSize()) {
        return false;
    }

    //Fist, resize to the appropriate size. All new values will be set to nullptr; If it fails, fail;
    if (!resizeTo(getSize() + src.getSize())) {
        return false;
    }

    T **src_p = src.elements;
    T **dst_p = elements + size;

    //Now, we must copy src's pointers in our array;
    for (uint8_t i = src_size; i--;) {

        //Copy the pointer of src in dst;
        *(dst_p++) = *(src_p++);

    }

    //Nullify src's content;
    src.elements = nullptr;
    src.size = 0;

    //Succeed;
    return true;

}


/*
 * rightConcatenation : concatenates the given rvalue container on the left;
 */

template<typename T>
bool DynamicPointerBuffer<T>::leftConcatenation(DynamicPointerBuffer<T> &&src) {

    //Cache current sizes;
    uint8_t size = this->size;
    uint8_t src_size = src.size;

    //First, determine the final size;
    uint16_t final_size = (uint16_t) size + (uint16_t) src.getSize();

    //If the concatenation can't be done : fail;
    if (final_size > (uint16_t) getMaxSize()) {
        return false;
    }

    //Fist, resize to the appropriate size. All new values will be set to nullptr; If it fails, fail;
    if (!resizeTo(getSize() + src.getSize())) {
        return false;
    }

    //We must shift our data;
    T **src_p = elements;
    T **dst_p = elements + src_size;

    for (uint8_t i = size; i--;) {

        //Copy the pointer of src in dst;
        *(dst_p++) = *(src_p++);

    }

    //Now, we must copy src's pointers in our array;

    src_p = src.elements;
    dst_p = elements;

    for (uint8_t i = src_size; i--;) {

        //Copy the pointer of src in dst;
        *(dst_p++) = *(src_p++);

    }

    //Nullify src's content;
    src.elements = nullptr;
    src.size = 0;

    //Succeed;
    return true;

}

//------------------------------------------------- Getters - Setters --------------------------------------------------


/*
 * getSize : this function returns the size of the list;
 */

template<class T>
uint8_t DynamicPointerBuffer<T>::getSize() const {

    //Return the size lol;
    return size;

}


/*
 * getMaxSize : this function returns the size of the list;
 */

template<class T>
uint8_t DynamicPointerBuffer<T>::getMaxSize() const {

    //Return the size lol;
    return maxSize;

}


/*
 * get_task : this function returns the task at the given index, if it is valid;
 */

template<class T>
T *DynamicPointerBuffer<T>::getElement(uint8_t index) const {

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
T *DynamicPointerBuffer<T>::setElement(uint8_t index, T *new_element) {

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


//-------------------------------------- Sort --------------------------------------

/*
 * sort : this function will sort the container, using a simple descending  bubble sort,
 *  based on the < operator of the class T;
 */

template<typename T>
void DynamicPointerBuffer<T>::sort() {

    //First, let's cache the size;
    uint8_t size = this->size;

    //If no elements are to sort, do nothing;
    if (!size) return;

    //For all elements except the first :
    for (uint8_t sort_index = size; --sort_index;) {

        T **down_swap = elements;
        T **up_swap = elements + 1;


        //For all elements before the sort index :
        for (uint8_t swap_count = sort_index; swap_count--;up_swap++, down_swap++) {

            //If the object at up index is higher than the one at down index :
            if ((*up_swap)->operator<(**down_swap)) {

                //Swap pointers;
                T* ptr = *up_swap;
                *up_swap = *down_swap;
                *down_swap = ptr;

            }

        }

    }

}


//-------------------------------------- Resizing --------------------------------------


/*
 * resize : this function will attempt to resize the array to the provided size;
 *
 * It will automatically delete removed elements, and initialise added elements to nullptr;
 */

template<class T>
bool DynamicPointerBuffer<T>::resizeTo(uint8_t new_size) {

    //If the maximum number of tasks is reached :
    if (new_size >= maxSize) {

        //Log;
        //std_out("Error in DynamicPointerBuffer::resize : the requested size is superior to the maximum size;");

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
    void *new_array = realloc(elements, new_size * sizeof(T));

    //If the reallocation failed :
    if (!new_array && new_size) {

        //Log;
        //std_out("Error in DynamicPointerBuffer::resize : the reallocation failed;");

        //Fail;
        return false;

    }


    //If the reallocation completed :

    //Update the tasks array;
    elements = (T **) new_array;


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