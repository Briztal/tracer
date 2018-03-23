
#ifndef TRACER_BUFFER_CPP
#define TRACER_BUFFER_CPP

#include "Buffer.h"


//---------------------------------- Initialisation ----------------------------------

/*
 * Constructor : takes the size in argument;
 */

template<typename T>
Buffer<T>::Buffer(uint8_t size) : max_size(size), buffer(new T[size]), data_pointer(buffer), nb_elements(0),
                                  nb_spaces(size) {

}


//---------------------------------- Data manipulation ----------------------------------


/*
 * add : copies the T to the current space;
 */

template<typename T>
bool Buffer<T>::add(const T &data) {

    //If there is still some space;
    if (nb_spaces) {

        //Copy the data in the current space;
        *(data_pointer++) = data;

        //Decrease the number of spaces and increment the number of elements;
        nb_spaces--, nb_elements++;

        //Complete;
        return true;

    }

    //If no spaces are available, fail;
    return false;

}


/*
 * add : copies the T to the current space;
 */

template<typename T>
bool Buffer<T>::safeAdd(const T &data) {

    //If there is still some space;
    if (nb_spaces > 1) {

        //Copy the data in the current space;
        *(data_pointer++) = data;

        //Decrease the number of spaces and increment the number of elements;
        nb_spaces--, nb_elements++;

        //Complete;
        return true;

    }

    //If no spaces are available, fail;
    return false;

}


/*
 * clear : resets the data pointer and counters;
 */

template<typename T>
void Buffer<T>::clear() {

    nb_elements = 0;
    nb_spaces = max_size;
    data_pointer = buffer;

}

//---------------------------------- Getters ----------------------------------


/*
 * size : this function returns the number of elements in the bufer;
 */

template<typename T>
uint8_t Buffer<T>::size() {

    //Return the number of elements in the queue;
    return nb_elements;

}


/*
 * data : returns a pointer to the first element of the array;
 */

template<typename T>
const T *Buffer<T>::data() {
    return buffer;
}

#endif