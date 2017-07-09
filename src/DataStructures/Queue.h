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

public:
    Queue(unsigned char size);

    T pull();

    void push(T);

    unsigned char available_spaces();

    unsigned char available_elements();

    const unsigned char pull_indice();

    const unsigned char push_indice();

private:

    const unsigned char size;
    const unsigned char max_indice;

    T *const content;

    unsigned char pull_index = 0;
    unsigned char push_index = 0;

    unsigned char elements = 0;
    unsigned char spaces;

};

template class Queue<motion_data>;
template class Queue<linear_data>;
template class Queue<int>;
template class Queue<void(*)(char *args, unsigned char args_size)>;
template class Queue<void(*)()>;

#endif //TRACER_QUEUE_H
