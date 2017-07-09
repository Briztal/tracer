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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef TRACER_ARGQUEUE_H
#define TRACER_ARGQUEUE_H


class ArgQueue {

public:

    ArgQueue(unsigned char size);

    unsigned char get();

    char *get_data_pointer();

    void add_data(char *data_in, unsigned char size);

private:

    const unsigned char size;
    unsigned char available;
    unsigned char spaces;

    unsigned char read_indice, write_indice;

    char *data_pointer;
    char *const data;

    unsigned char available_space();
};


#endif
