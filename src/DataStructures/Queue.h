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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

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
    Queue(unsigned char size)  :
            size(size), content(new T[size]), max_indice((const unsigned char) (size - 1)),
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

    unsigned char available_spaces() {
        return spaces;
    }

    unsigned char available_elements() {
        return elements;
    }

    const unsigned char pull_indice() {
        return pull_index;
    }

    const unsigned char push_indice() {
        return push_index;
    }

private:

    const unsigned char size;
    const unsigned char max_indice;

    T *const content;

    unsigned char pull_index = 0;
    unsigned char push_index = 0;

    unsigned char elements = 0;
    unsigned char spaces;

};


#endif //TRACER_QUEUE_H
