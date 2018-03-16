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

#include <DataStructures/string/tstring.h>
#include <DataStructures/Containers/Buffer.h>


class Delimiter {

    //----------------------------------------- Decoding -----------------------------------------

public:

    //Constructor : takes the size and initialises the buffer;
    explicit Delimiter(uint8_t size);

    //Destructor : no dynamic data to delete;
    virtual ~Delimiter() = default;


    //----------------------------------------- Decoding -----------------------------------------

public:

    //process a char;
    virtual void process(char data) = 0;

    //Can the data be parsed ?
    bool isMessageDecoded();

    //Get a pointer to the data;
    const char *getData();

    //clear the input environment;
    void clear();


    //----------------------------------------- Encoding -----------------------------------------

public:

    //Encode a message;
    virtual void encode(tstring &) = 0;


    //----------------------------------------- Special Methods -----------------------------------------

protected :

    //The buffer that will contain the received data;
    Buffer<char> receptionBuffer;

    //The parsing flag. Set when received data can be parsed;
    bool messageDecoded = false;

    //A flag set if the current packet is corrupted (too long for the data buffer);
    bool corruptedPacket = false;

};

#endif //TRACER_PROTOCOL_H
