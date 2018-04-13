//
// Created by root on 4/9/18.
//


#ifndef TRACER_OBJECTCONTAINE_CPP
#define TRACER_OBJECTCONTAINE_CPP

#include "ObjectContainer.h"

//--------------------------- Construction ---------------------------

/*
 * Constructor : initialises the superclass;
 */

template<class T>
ObjectContainer<T>::ObjectContainer(uint8_t max_size) : Container<T*>(max_size) {

}


/*
 * Copy constructor : duplicates the array and duplicates all elements;
 */

template<class T>
ObjectContainer<T>::ObjectContainer(const ObjectContainer<T> &src): Container<T*>(src) {

    //For each object :
    for (uint8_t object_id = Container<T*>::size; object_id--;) {

        //Duplicate the object and store it;
        Container<T*>::data[object_id] = new T(Container<T*>::data[object_id]);

    }

}


/*
 * Move constructor : moves the array;
 */

template<class T>
ObjectContainer<T>::ObjectContainer(ObjectContainer<T> &&src) noexcept : Container<T*>((Container<T*> &&)src){}


/*
 * Destructor : deletes all objects;
 */

template<class T>
ObjectContainer<T>::~ObjectContainer() {

    //For each object :
    for (uint8_t object_id = Container<T*>::size; object_id--;) {

        //Delete the object;
        delete Container<T*>::data[object_id];

    }

    //The array deletion is accomplished by the parent constructor;

}


//--------------------------- Assignment ---------------------------

/*
 * Copy assignment operator : duplicates the array and duplicates every object;
 */

template<class T>
ObjectContainer<T>& ObjectContainer<T>::operator=(const ObjectContainer<T> &src) {

    //Call parent operator;
    Container<T*>::operator=(src);

    //For each object :
    for (uint8_t object_id = Container<T*>::size; object_id--;) {

        //Duplicate the object and store it;
        Container<T*>::data[object_id] = new T(Container<T*>::data[object_id]);

    }

    //Return a pointer to us;
    return *this;

}


/*
 * Move assignment operator : calls the parent move assimngment operator;
 */

template<class T>
ObjectContainer<T>& ObjectContainer<T>::operator=(ObjectContainer<T> &&src) noexcept {

    //Call parent operator;
    Container<T*>::operator=((Container<T*> &&) src);

    //Return a pointer to us;
    return *this;

}


//--------------------------- Getters ---------------------------

/*
 * getSize : returns the size of the container;
 */
template<typename T>
uint8_t ObjectContainer<T>::getSize() {

    return Container<T*>::getSize();

}


/*
 * get : get a pointer to an element;
 */

template<class T>
T *ObjectContainer<T>::get(uint8_t element_index) {

    //If the index is invalid, fail;
    if (element_index >= Container<T>::size)
        return nullptr;

    //Return the pointer to the required element;
    return Container<T>::data[element_index];

}



//--------------------------- Operations ---------------------------


/*
 * Add by reference. Creates a heap copy;
 */

template<class T>
bool ObjectContainer<T>::add(uint8_t index, T &element) {

    //Create a heap-copy and add it;
    return Container<T*>::add(index, new T(element));

}


/*
 * Add by rvalue; Creates a heap-move copy and adds it;
 */

template<class T>
bool ObjectContainer<T>::add(uint8_t index, T &&element) {

    //Create a heap-copy and add it;
    return Container<T*>::add(index, new T((T &&)element));

}


/*
 * Add by reference. Creates a heap copy;
 */

template<class T>
void ObjectContainer<T>::add(T &element) {

    //Create a heap-copy and add it;
    return Container<T*>::add(new T(element));

}


/*
 * Add by rvalue; Creates a heap-move copy and adds it;
 */

template<class T>
void ObjectContainer<T>::add(T &&element) {

    //Create a heap-copy and add it;
    return Container<T*>::add(new T((T &&)element));

}


/*
 * Set by reference : creates a heap copy, sets the required element and delete the replaced object;
 */

template<class T>
void ObjectContainer<T>::set(uint8_t index, T &element) {

    //Update the pointer with a heap-copy and delete the replaced object;
    delete Container<T*>::set(index, new T(element));

}


/*
 * Set by rvalue : creates a heap moved copy, sets the required element and delete the replaced object;
 */

template<class T>
void ObjectContainer<T>::set(uint8_t index, T &&element) {

    //Update the pointer with a heap-copy and delete the replaced object;
    delete Container<T*>::set(index, new T((T&&)element));

}


/*
 * remove : removes the required object;
 */

template<class T>
void ObjectContainer<T>::remove(uint8_t index) {

    //Remove the pointer at the given index and delete the object it references;
    delete Container<T*>::remove(index);

}


/*
 * clear : deletes each object and clear the container;
 */

template<class T>
void ObjectContainer<T>::clear() {

    //For each object :
    for (uint8_t object_id = Container<T*>::size; object_id--;) {

        //Delete the object;
        delete Container<T*>::data[object_id];

    }

    //Clear the container
    Container<T*>::clear();

}



/*
 * Resize : this function will try to reallocate the array to the required size;
 */

template<typename T>
bool ObjectContainer<T>::resize(uint8_t new_size) {

    //Cache the current old_size;
    const uint8_t old_size = Container<T*>::size;

    //First, let's delete all objects that will be removed from the container;
    for (uint8_t object_ptr = new_size; object_ptr < old_size; object_ptr++) {

        //Remove the object;
        delete Container<T*>::data[object_ptr];

    }

    //Now, resize the array;
    Container<T*>::resize(new_size);

    //Update the new size to the real one;
    new_size = Container<T*>::size;

    //Finally, set nullptr in all new cases;
    for (uint8_t object_ptr = old_size; object_ptr < new_size; object_ptr++) {
        Container<T*>::data[object_ptr] = nullptr;
    }

    //Complete;
    return true;

}


#endif //TRACER_OBJECTCONTAINE_CPP
