/*
  ArgQueue.h - Part of TRACER

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

#ifndef TRACER_ARGQUEUE_H
#define TRACER_ARGQUEUE_H


#include <stdint.h>

class ArgQueue {

public:

    ArgQueue(uint8_t size);

    uint8_t get();

    char *get_data_pointer();

    void add_data(char *data_in, uint8_t size);

private:

    const uint8_t size;
    uint8_t available;
    uint8_t spaces;

    uint8_t read_indice, write_indice;

    char *data_pointer;
    char *const data;

    uint8_t available_space();
};


#endif
