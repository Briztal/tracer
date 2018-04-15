//
// Created by root on 4/9/18.
//

#ifndef TRACER_OBJECTCONTAINE_H
#define TRACER_OBJECTCONTAINE_H

#include "Container.h"

template <class T>
class ObjectContainer : protected Container<T*> {

    //--------------------------- Construction ---------------------------

public:

    //Default constructor;
    explicit ObjectContainer(uint8_t max_size);

    //Copy constructor;
    ObjectContainer(const ObjectContainer<T> &src);

    //Move constructor;
    ObjectContainer(ObjectContainer<T> &&src) noexcept ;

    //Destructor;
    ~ObjectContainer();


    //--------------------------- Assignment ---------------------------

public:

    //Copy assignment operator;
    ObjectContainer<T>& operator=(const ObjectContainer<T> &src);

    //Move assignment operator;
    ObjectContainer<T>& operator=(ObjectContainer<T> &&src) noexcept;


    //--------------------------- Getters ---------------------------

public:

    //Get the size of the container;
    uint8_t getSize();

    //Get a pointer to an element;
    T* get(uint8_t element_index);


    //--------------------------- Setters ---------------------------

public:

    //Insert an element at a given index;
    bool add(uint8_t index, T& element);

    //Insert an element at a given index;
    bool add(uint8_t index, T&& element);

    //Append an element to the array;
    void add(T &element);

    //Append an element to the array;
    void add(T &&element);

    //Update an element of the array;
    void set(uint8_t index, T& element);

    //Update an element of the array;
    void set(uint8_t index, T&& element);

    //Remove an element of the array;
    void remove(uint8_t index);

    //Clear the data of the array;
    void clear();



    //--------------------------- Resize ---------------------------

private:

    //Resize to the given size;
    bool resize(uint8_t new_size) final;


};

#include "ObjectContainer.cpp"


#endif //TRACER_OBJECTCONTAINE_H
