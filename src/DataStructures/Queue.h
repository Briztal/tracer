/*
  Queue.h - Part of TRACER

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

#ifndef TRACER_QUEUE_H
#define TRACER_QUEUE_H

#include "../StepperControl/motion_data.h"

template <typename T> class Queue {


#define SAFE_DECR(i)\
    if (i) {\
        i--;\
    } else {\
        i = max_indice;\
    }\


public:
    Queue(uint8_t size)  :
            size(size), content(new T[size]), max_indice((const uint8_t) (size - 1)),
            spaces(size){

    }


    T pull() {

        T element = content[pull_index];

        SAFE_DECR(pull_index);

        elements--;
        spaces++;

        return element;
    }

    void push(T element) {

        if (!spaces)
            return;

        content[push_index] = element;

        SAFE_DECR(push_index);

        elements++;
        spaces--;

    }

    T * peak_pushed() {
        if (push_index != max_indice) {
            return content+push_index+1;
        } else {
            return content;
        }
    }

    uint8_t available_spaces() {
        return spaces;
    }

    uint8_t available_elements() {
        return elements;
    }

    const uint8_t pull_indice() {
        return pull_index;
    }

    const uint8_t push_indice() {
        return push_index;
    }

private:

    const uint8_t size;
    const uint8_t max_indice;

    T *const content;

    uint8_t pull_index = 0;
    uint8_t push_index = 0;

    uint8_t elements = 0;
    uint8_t spaces;

};


#endif //TRACER_QUEUE_H
