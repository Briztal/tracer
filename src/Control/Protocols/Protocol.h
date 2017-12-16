/*
  Protocol.h - Part of TRACER

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


#ifndef TRACER_PROTOCOL_H
#define TRACER_PROTOCOL_H

#include "hardware_language_abstraction.h"

class Protocol {

public:

    Protocol()  :
            buffer_size(0), command_size(0), data_spaces(0), corrupted_packet(false),
            data_in_0(new char[0]),
            data_available(nullptr), read_data(nullptr), write_data(nullptr) {

        //Initialise the data pointer to the first case;
        data_in = data_in_0;

    }

    Protocol(uint8_t size, uint16_t (*const available_f)(void), void (*const write_f)(char),
                         char (*const read_f)(void)) :
            buffer_size(size), command_size(size), data_spaces(size), corrupted_packet(false),
            data_in_0(new char[size]),
            data_available(available_f), read_data(read_f), write_data(write_f) {

        //Initialise the data pointer to the first case;
        data_in = data_in_0;

    }

    //read all available_data;
    virtual void decode_data() {}

    //Encode a string of char;
    virtual void encode_data(String data) {}

    //Read_data data on the data_link;
    virtual void decode_char(char data) {}

    //Get a pointer to the data;
    virtual char *get_data() {}

    //Can the data be parsed ?
    virtual bool parsing_ready() {}

    //Reset the reading/parsing environment;
    virtual void reset() {}


    //----------------------------------------- Special Methods -----------------------------------------

protected:

    //The method to know how many bytes are available;
    uint16_t (*const data_available)(void) = 0;

    //The method to write_data a char;
    void (*const write_data)(char) = 0;

    //The method to read_data a char;
    char (*const read_data)() = 0;


protected :

    //The parsing flag. Set when received data can be parsed;
    bool packet_received = false;

    //The buffer size;
    const uint8_t buffer_size;

    //The current command's size;
    uint8_t command_size;

    //The current number of available spaces in the data buffer;
    uint8_t data_spaces;

    //A flag set if the current packet is corrupted (too long for the data buffer);
    bool corrupted_packet = false;

    //The current data pointer, points to a case in the data buffer;
    char *data_in;

    //The beginning of the data buffer;
    char *const data_in_0;



};

#endif //TRACER_PROTOCOL_H
