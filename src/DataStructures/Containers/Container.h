//
// Created by root on 3/25/18.
//

#ifndef TRACER_CONTAINER_H
#define TRACER_CONTAINER_H

#include "stdint.h"

template <typename T>
class Container {


    //--------------------------- Construction ---------------------------

public:

    //Default constructor;
    Container();

    //Copy constructor;
    Container(const Container<T> &src);

    //Move constructor;
    Container(Container<T> &&src) noexcept ;

    //Destructor;
    ~Container();


    //--------------------------- Assignment ---------------------------

public:

    //Copy assignment operator;
    void operator=(Container<T> &src);

    //Move assignment operator;
    void operator=(Container<T> &&src);


    //--------------------------- Operations ---------------------------

public:

    //Insert an element at a given index;
    bool add(uint8_t index, T element);

    //Append an element to the array;
    void add(T element);

    //Update an element of the array;
    void set(uint8_t index, T element);

    //Remove an element of the array;
    void remove(uint8_t index);

    //Clear the content of the array;
    void clear();


    //--------------------------- Resize ---------------------------


private:

    //Resize to the given size;
    bool resize(uint8_t new_size);


    //--------------------------- Data ---------------------------

private:

    //The array of data;
    T* data;

    //The size of the array;
    uint8_t size;


};


#endif //TRACER_CONTAINER_H
