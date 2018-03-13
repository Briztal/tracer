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
#include "Interaction/Protocols/Protocol.h"

class ClearText : public Protocol {

public :

    //Constructor;
    ClearText(uint8_t size, uint16_t (*const available_f)(void), void (*const write_f)(char), char (*const read_f)(void));

    ~ClearText() {};

    //read all available_data;
    void decode_data();

    //Encode data on data_link;
    void encode_data(string data);

    //Get a pointer to the data;
    char *get_data();

    //Is the data can be parsed;
    bool parsing_ready();

    //Reset the reading/parsing environment.
    void reset();


private:

    //read_data data on the data_link
    void decode_char(char data);


};


#endif //TRACER_CLEARTEXT_H
