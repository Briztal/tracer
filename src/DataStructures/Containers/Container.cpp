//
// Created by root on 3/25/18.
//

#include <DataStructures/string/string.h>
#include "Container.h"


//--------------------------- Construction ---------------------------

/*
 * Default Constructor : initialises the data to an empty array;
 */

template<typename T>
Container<T>::Container() : data(nullptr), size(0) {}


/*
 * Copy Constructor : copies the data of the given array;
 */

template<typename T>
Container<T>::Container(const Container<T> &src) : Container() {

    //Resize to the given array's size;
    if (resize(src.size)) {

        //If the resizing succeeded, copy the content;
        mmemcpy(data, src.data, sizeof(T) * size);

    }

}


/*
 * Move constructor : moves the data of the given array;
 */

template<typename T>
Container<T>::Container(Container<T> &&src) : data(src.data), size(src.size) {

    //Reset the source's data;
    src.data = nullptr;
    src.size = 0;

}


/*
 * Destructor : free the content of the array;
 */

template<typename T>
Container<T>::~Container() {

    //Free the data array;
    free(data);

}


//--------------------------- Assignment ---------------------------

/*
 * Copy assignment operator : frees our data and copies the other's;
 */

template<typename T>
void Container<T>::operator=(Container<T> &src) {

    //If we try to assign ourselves, nothing to do;
    if (&src == this)
        return;

    //Resize to the given array's size;
    if (resize(src.size)) {

        //If the resizing succeeded, copy the content;
        mmemcpy(data, src.data, sizeof(T) * size);

    }

}

/*
 * Move assignment operator;
 */

template<typename T>
void Container<T>::operator=(Container<T> &&src) noexcept {

    //Move the size and the data pointer;
    size = src.size;
    src.size = 0;
    data = src.data;
    src.data = 0;

}

//--------------------------- Operations ---------------------------

/*
 * add : insert an element at the given position of the array;
 *
 * This function starts to realloc the array, and if it succeeds, saves the new element;
 */

template<typename T>
bool Container<T>::add(uint8_t index, T element) {

    //If the index is invalid, fail;
    if (index > size)
        return false;

    //If the resizing completed :
    if (resize(index + (uint8_t) 1)) {

        //Cache the shift index;
        T *shift_ptr = data + size - (uint8_t)2;

        //All value has been reallocated, now we must shift value from the insertion index;
        for (uint8_t shift_counts = size - (index + (uint8_t)1); shift_counts--;) {

            //Shift the value;
            *(shift_ptr + 1) = *shift_ptr;

            //Update the the pointer;
            shift_ptr--;

        }

        //Save the element;
        element[index] = element;

        //Complete;
        return true;

    }

    //Fail if the reallocation failed;
    return false;

}


/*
 * add : appends a value to the array;
 */

template<typename T>
void Container<T>::add(T element) {

    //Add the element at the end of the array;
    add(size - (uint8_t)1, element);

}

/*
 * set : sets the
 */
template<typename T>
T Container<T>::set(uint8_t index, T element) {

    //If the index is invalid, fail;
    if (index >= size) {
        return 0;
    }

    //Save the old element;
    T old = data[index];

    //Set the element;
    data[index] = element;

    //Return the old element;
    return old;


}


/*
 * Remove : this function will //TODO COMMENT SHIFT AND REDACT;
 */
template<typename T>
T Container<T>::remove(uint8_t index) {



}

template<typename T>
void Container<T>::clear() {

}
