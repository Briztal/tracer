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


#ifndef TRACER_DYNAMICSET_H
#define TRACER_DYNAMICSET_H

#include "DynamicBuffer.h"

template<typename T>
class DynamicSet : public DynamicBuffer<T> {

    //---------------------------------------------- Initialisation ----------------------------------------------

public:

    //Constructor;
    explicit DynamicSet(uint8_t max_size);

    //Destructor;
    virtual ~DynamicSet() = default;


    //---------------------------------------- Copy Constructor ----------------------------------------

public:

    //Copy constructor;
    DynamicSet(const DynamicSet &container) : DynamicBuffer<T>(container){};

    //Move constructor;
    DynamicSet(DynamicSet &&container) noexcept : DynamicBuffer<T>(container){};


    //---------------------------------------- Assignment operator ----------------------------------------

    //Copy assignment operator;
    DynamicSet &operator=(const DynamicSet &container) {DynamicBuffer<T>::operator=(container);}

    //Copy assignment operator;
    DynamicSet &operator=(DynamicSet &&container) noexcept {DynamicBuffer<T>::operator=(container);};


    //---------------------------------------------- Functions ----------------------------------------------

public:

    //Add a task;
    bool add(T new_element) override;

    //Remove a task by object, return true if it was removed;
    bool remove(T element);

    //Remove a task by object, return true if it was removed, and eventually save its old index;
    bool remove(T element, uint8_t *old_index);

};

#include "DynamicSet.cpp"

#endif //TRACER_DYNAMICSET_H
