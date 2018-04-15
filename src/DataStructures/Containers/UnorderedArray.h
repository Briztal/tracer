//
// Created by root on 4/11/18.
//

#ifndef TRACER_UNORDEREDARRAY_H
#define TRACER_UNORDEREDARRAY_H

#include "stdint.h"

template<typename T>
class UnorderedArray {

    //--------------- Initialisation ---------------

public:

    //Constructor;
    UnorderedArray(uint8_t size);

    //Copy Constructor;
    UnorderedArray(const UnorderedArray &src);

    //Move Constructor:
    UnorderedArray(UnorderedArray &&src) noexcept;

    //Destructor;
    ~UnorderedArray();


    //--------------- Operations ---------------

public:

    //Add an element and get its index;
    uint8_t add(T &element);

    //Get the element at the required index;
    T *get(uint8_t index);

    //Remove the elementy at the required index;
    void remove(uint8_t index);


    //--------------- Getters ---------------

public:

    //Get the total size of the array (constant);
    uint8_t getSize() const;

    //Get the number of allocated elements in the array (variable);
    uint8_t getNbElements() const;

    //Get the number of unallocated elements in the array (variable);
    uint8_t getNbSpaces() const;


    //--------------- Fields ---------------

private:

    /*
     * As an allocation check is mandtory, the array itself will contain special elements, as defined here :
     */

    struct ArrayElement {

        //The element;
        T element;

        //An allocation flag, at the end for alignment;
        bool allocation_flag = false;

    };

    //The data array;
    ArrayElement *data;

    //The size of the array;
    const uint8_t size;

    //The number of elements allocated in the array;
    uint8_t nbElements;

    //The number of unallocated elements;
    uint8_t nbSpaces;



};

#include "UnorderedArray.cpp"


#endif //TRACER_UNORDEREDARRAY_H
