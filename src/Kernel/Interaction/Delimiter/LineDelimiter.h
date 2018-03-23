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


#ifndef TRACER_LINEDELIMITER_H
#define TRACER_LINEDELIMITER_H

#include "stdint.h"
#include "Kernel/Interaction/Delimiter/Delimiter.h"


/*
 * LineDelimiter : this delimiter recognised line messages.
 *
 *  More precisely, it will detect a message everytime a non-standard char is received,
 *      and encodes a message by adding \n\r at the end of the string;
 */

class LineDelimiter : public Delimiter {

    //----------------------------------------- Decoding -----------------------------------------

public:

    //Constructor : takes the size and initialises the buffer;
    explicit LineDelimiter(uint8_t size);

    //Destructor : no dynamic data to delete;
    virtual ~LineDelimiter() = default;

    //----------------------------------------- Decoding -----------------------------------------

    //Decode a char;
    void process(char data) final;


    //----------------------------------------- Encoding -----------------------------------------

public:

    //Encode a message;
    virtual void encode(tstring &) final;


};


#endif //TRACER_LINEDELIMITER_H
