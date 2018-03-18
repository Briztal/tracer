//
// Created by root on 3/15/18.
//

#ifndef TRACER_BUFFER_H
#define TRACER_BUFFER_H

#include "stdint.h"

template<typename T>
class Buffer {

    //---------------------------------- Initialisation ----------------------------------

public:

    //Constructor with size;
    explicit Buffer(uint8_t size);


    //---------------------------------- Data manipulation ----------------------------------

public:

    //Add a T to the buffer;
    bool add(const T &);

    //Add a T to the buffer, only if it doesn't fill the buffer;
    bool safeAdd(const T &);

    //Clear the buffer
    void clear();


    //---------------------------------- Getters ----------------------------------

    //Get the data size;
    uint8_t size();

    //Get the data pointer;
    const T *data();


    //---------------------------------- Constant data ----------------------------------


    //The size of the buffer;
    const uint8_t max_size;
    //The first case of the buffer;
    T *const buffer;



    //---------------------------------- Data ----------------------------------

    //The current case of the buffer;
    T *data_pointer;

    //The current number of elements of the array;
    uint8_t nb_elements;

    //The current number of spaces of the array;
    uint8_t nb_spaces;


};


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


#endif //TRACER_BUFFER_H
