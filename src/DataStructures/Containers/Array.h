//
// Created by root on 3/15/18.
//

#ifndef TRACER_BUFFER_H
#define TRACER_BUFFER_H

#include "stdint.h"

template<typename T>
class Array {

    //---------------------------------- Initialisation ----------------------------------

public:

    //Constructor with size;
    explicit Array(uint8_t size);


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

#include "Array.cpp"

#endif //TRACER_BUFFER_H
