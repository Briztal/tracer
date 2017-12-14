/*
  ClearText.h - Part of TRACER

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


#ifndef TRACER_CLEARTEXT_H
#define TRACER_CLEARTEXT_H

#include "stdint.h"

class ClearText {


public :
    //read data on the data_link
    static bool read_data();

    //Reset the reading/parsing environment.
    static void reset();


private :

    //The current command's size;
    static uint8_t command_size;

    //The current number of available spaces in the data buffer;
    static uint8_t data_spaces;

    //A flag set if the current packet is corrupted (too long for the data buffer);
    static bool corrupted_packet;

    //The current data pointer, points to a case in the data buffer;
    static char *data_in;

    //The beginning of the data buffer;
    static char *const data_in_0;

};


#endif //TRACER_CLEARTEXT_H
