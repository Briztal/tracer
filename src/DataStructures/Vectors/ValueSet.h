/*
  ValueSet.h - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/


#ifndef TRACER_VALUESET_H
#define TRACER_VALUESET_H

#include "ValueVector.h"

template<typename T>
class ValueSet : public ValueVector<T> {

public:

    //Default constructor;
    explicit ValueSet(uint8_t max_size);

    //Add a task;
    bool add(T new_element) override;

    //Remove a task by object;
    void remove(T element);

};


/*
 * Constructor calls the parent constructor;
 */

template<class T>
ValueSet<T>::ValueSet(uint8_t max_size) : ValueVector<T>(max_size) {}


/*
 * add : this method overrides the add method, as a function can't be added twice;
 */

template<class T>
bool ValueSet<T>::add(T new_element) {

    //The index to pass to isElementPresent. Unused;
    uint8_t index = 0;

    //If the element is already present :
    if (ValueVector<T>::isElementPresent(new_element, &index)) {

        //Fail;
        return false;

    }

    //If it is not present, try to add it;
    return ValueVector<T>::add(new_element);

}


template<class T>
void ValueSet<T>::remove(T element) {

    //First, we must determine the eventual index of the element;
    uint8_t index = 0;

    //If the element is not present :
    if (!ValueVector<T>::isElementPresent(element, &index)) {

        //Fail, nothing to remove;
        return;

    }

    ValueVector<T>::remove(index);

}


#endif //TRACER_VALUESET_H
