/*
  Queue.cpp - Part of TRACER

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

/*

#include "Queue.h"

#define SAFE_DECR(i)\
    if (i) {\
        i--;\
    } else {\
        i = max_indice;\
    }\


template <typename T> Queue<T>::Queue(uint8_t size) :
        size(size), content(new T[size]), max_indice((const uint8_t) (size - 1)),
        spaces(size){

}

template <typename T> const uint8_t Queue<T>::pull_indice() {
    return pull_index;
}

template <typename T> const uint8_t Queue<T>::push_indice() {
    return push_index;
}

template <typename T> T Queue<T>::pull() {

    T element = content[pull_index];

    SAFE_DECR(pull_index);

    elements--;
    spaces++;

    return element;
}

template <typename T> void Queue<T>::push(T element) {

    if (!spaces)
        return;

    content[push_index] = element;

    SAFE_DECR(push_index);

    elements++;
    spaces--;

}

template <typename T> uint8_t Queue<T>::available_spaces() {
    return spaces;
}

template <typename T> uint8_t Queue<T>::available_elements() {
    return elements;
}

*/
