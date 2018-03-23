/*
  ClearText.cpp - Part of TRACER

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

#include "LineDelimiter.h"
#include <Kernel/Interaction/Interaction.h>


/*
 * Constructor : initialises all fields;
 */

LineDelimiter::LineDelimiter(uint8_t size) : Delimiter(size) {}


/*
 * decode_char : this function reads and processes data.
 *
 *  It reads data on the link layer, saves it, and eventually processes it.
 */

void LineDelimiter::process(char read_char) {

    //Don't process any data if no space is available in the argument_t sequence container :
    if (messageDecoded) {

        //Fail;
        return;

    }

    //If the received char is a non standard character;
    if (read_char < 32) {

        //If the received packet was too long for the input buffer :
        if (corruptedPacket) {

            //Display an error message
            std_out("WARNING in Terminal::decode_char : the received packet was too long for "
                             "the input buffer. Please check your terminal_interface_config.h");

        } else if (receptionBuffer.size()) {
            //If a non empty uncorrupted packet has effectively been received :

            //Enable the parsing flag;
            messageDecoded = true;

            //Set the last byte to 0;
            receptionBuffer.add(0);

            //Complete;
            return;

        }


        //If the packet was corrupted, or empty;
        clear();


    } else {

        //If the packet hasn't been entirely received :
        if ((!corruptedPacket) && !receptionBuffer.safeAdd(read_char)) {

            //Mark the current packet as corrupted;
            corruptedPacket = true;

        }

    }

}


/*
 * encode : this function will append "\n\r" at the end of the string;
 */

void LineDelimiter::encode(tstring &data) {

    //Add the termination;
    data+=string("\n\r");


}


#define m ClearText



