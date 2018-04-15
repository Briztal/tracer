//
// Created by root on 4/11/18.
//

#ifndef TRACER_UNORDEREDARRAY_CPP
#define TRACER_UNORDEREDARRAY_CPP

#include "UnorderedArray.h"

#include <DataStructures/string/string.h>


//--------------- Initialisation ---------------


/*
 * Base constructor : initialises the array to the required size;
 *
 *  All allocation flags are set to false by definition;
 */

template<typename T>
UnorderedArray<T>::UnorderedArray(uint8_t size) :
        data(new ArrayElement[size]), size(size), nbElements(0), nbSpaces(size) {}


/*
 * Copy constructor : Initialises the array to the same size as the provided one, and clones its content;
 */

template<typename T>
UnorderedArray<T>::UnorderedArray(const UnorderedArray &src) : UnorderedArray(src.size) {

    //Copy the content of the source array in ours;
    mmemcpy(data, src.data, sizeof(ArrayElement) * src.size);

}


/*
 * Move constructor : phagocytes the content of the source array, and resets it;
 */

template<typename T>
UnorderedArray<T>::UnorderedArray(UnorderedArray &&src) noexcept :
        data(src.data), size(src.size), nbElements(src.nbElements), nbSpaces(src.nbSpaces) {
    src.data = nullptr;
}


/*
 * Destructor : deletes the data array;
 */

template<typename T>
UnorderedArray<T>::~UnorderedArray() {

    //Only the array is to delete;
    delete data;

}


//--------------- Operations ---------------

/*
 * add : tries to find a space for the given element, and eventually adds it;
 */

template<typename T>
uint8_t UnorderedArray<T>::add(T &element) {

    //Iterate over all elements;
    for (uint8_t element_index = size; element_index--; ) {

        //Cache the pointer to the array element;
        ArrayElement *arrayElement = data + element_index;

        //If the element is not allocated;
        if (!arrayElement->allocation_flag) {

            //Copy the value in the element;
            arrayElement->element = element;

            //Mark the element as allocated;
            arrayElement->allocation_flag = true;

            //Increase the number of elements and decrease the number of spaces;
            nbElements++;
            nbSpaces--;

            //Return the given index;
            return element_index;

        }

    }

    //If all elements were allocated :
    //TODO USAGE FAULT;

    //Return a pointer to the first element for safety;
    return 0;

}


/*
 * get : returns a pointer to the element at the given index, if it is allocated;
 */

template<typename T>
T *UnorderedArray<T>::get(uint8_t index) {

    //If the index is invalid, fail; //TODO USAGE FAULT;
    if (index >= size)
        return &data[0].element;

    //If the required element is not allocated,  fail; //TODO USAGE FAULT;
    if (!data[index].allocation_flag)
        return &data[0].element;

    //Return a pointer to the required element;
    return &data[index].element;

}


/*
 * remove : marks the required element as unallocated;
 */

template<typename T>
void UnorderedArray<T>::remove(uint8_t index) {

    //If the index is invalid, fail; //TODO USAGE FAULT;
    if (index >= size)
        return;

    //If the required element is not allocated,  fail; //TODO USAGE FAULT;
    if (!data[index].allocation_flag)
        return;

    //Mark the required element as allocated;
    data[index].allocation_flag = false;

    //Decrease the number of elements and increase the number of spaces;
    nbElements--;
    nbSpaces++;
}


/*
 * getSize : returns the size;
 */

template<typename T>
uint8_t UnorderedArray<T>::getSize() const {

    //Return the size;
    return size;

}


/*
 * getNbElements : returns the number of allocated elements;
 */

template<typename T>
uint8_t UnorderedArray<T>::getNbElements() const {

    //Return the number of elements;
    return nbElements;

}


/*
 * getNbSpaces : returns the number of unallocated elements;
 */

template<typename T>
uint8_t UnorderedArray<T>::getNbSpaces() const {

    //Return the number of spaces;
    return nbSpaces;

}

#endif //TRACER_UNORDEREDARRAY_CPP