/*
  Delimiters.cpp - Part of TRACER

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


#include "Delimiter.h"

/*
 * Constructor : initialises the buffer;
 */

Delimiter::Delimiter(uint8_t size) : receptionBuffer(size) {}


//----------------------------------------- Decoding -----------------------------------------



/*
 * isMessageDecoded : will return the delimitation state;
 */

bool Delimiter::isMessageDecoded() {

    //Return the message decoding flag, updated by the process method;
    return messageDecoded;

}


/*
 * getData : return a pointer to the data buffer, and clears the delimitation flag;
 */
const char *Delimiter::getData() {

    //Clear the delimitation flag;
    messageDecoded = false;

    //Return a pointer to the data buffer;
    return receptionBuffer.data();

}


/*
 * clear : resets the delimitation environment;
 */

void Delimiter::clear() {

    //Clear the delimitation flag;
    messageDecoded = false;

    //Clear the reception buffer;
    receptionBuffer.clear();

    //Clear the corruption flag;
    corruptedPacket = false;

}
