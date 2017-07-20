/*
  ArgQueue.cpp - Part of TRACER

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
#include "ArgQueue.h"

ArgQueue::ArgQueue(uint8_t size) :
        data(new char[size]), size(size) {
    available = size;
    spaces = 0;
    read_indice = write_indice = size - (uint8_t) 1;
    data_pointer = data;
}

void ArgQueue::add_data(char *data_in, uint8_t size) {

}


uint8_t ArgQueue::get() {
    return 0;
}

char *ArgQueue::get_data_pointer() {

}

uint8_t ArgQueue::available_space() {
    return spaces;
}

