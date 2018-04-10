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

    virtual //Destructor;
    ~Container();


    //--------------------------- Assignment ---------------------------

public:

    //Copy assignment operator;
    Container<T>& operator=(const Container<T> &src);

    //Move assignment operator;
    Container<T>& operator=(Container<T> &&src) noexcept;



    //--------------------------- Getters ---------------------------

public:

    //Get the size of the container;
    uint8_t getSize();

    //Get a pointer to an element;
    T* getPointer(uint8_t element_index);


    //--------------------------- Operations ---------------------------

public:

    //Insert an element at a given index;
    bool add(uint8_t index, T element);

    //Append an element to the array;
    void add(T element);

    //Update an element of the array;
    T set(uint8_t index, T element);

    //Remove an element of the array;
    virtual void remove(uint8_t index);

    //Clear the content of the array;
    virtual void clear();


    //--------------------------- Resize ---------------------------


protected:

    //Resize to the given size;
    virtual bool resize(uint8_t new_size);


    //--------------------------- Data ---------------------------

protected :

    //The array of data;
    T* data;

    //The size of the array;
    uint8_t size;


};

#include "Container.cpp"

#endif //TRACER_CONTAINER_H
