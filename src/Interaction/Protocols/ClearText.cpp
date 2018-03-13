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

#include "ClearText.h"
#include <Interaction/Interaction.h>


/*
 * Constructor : initialises all fields;
 */

ClearText::ClearText(uint8_t size, uint16_t (*const available_f)(void), void (*const write_f)(char),
                     char (*const read_f)(void)) : Protocol(size, available_f, write_f, read_f) {

    reset();
}


/*
 * encode_data : this function encodes a string, and sends it.
 */

void ClearText::encode_data(string s) {

    //For every char in the string :
    for (uint8_t i = 0; i < s.length(); i++) {

        //Echo the char;
        (*write_data)(s.charAt(i));

    }
}


void ClearText::decode_data() {

    //While data is available and the parsing is not ready;
    while ((*data_available)() && !packet_received) {

        //Read a char and decode it.
        decode_char((*read_data)());
    }

}


/*
 * decode_char : this function reads and processes data.
 *
 *  It reads data on the link layer, saves it, and eventually processes it.
 */

void ClearText::decode_char(char read_char) {

    //Don't process any data if no space is available in the argument_t sequence container :
    if (packet_received) {

        //Fail;
        return;

    }

    //If the received char is a line feed or a working_extruder return :
    if (read_char < 32) {

        //If the received packet was too long for the input buffer :
        if (corrupted_packet) {

            //Display an error message
            std_out("WARNING in Terminal::decode_char : the received packet was too long for "
                             "the input buffer. Please check your terminal_interface_config.h");

        } else if (command_size) {
            //If a non empty uncorrupted packet has effectively been received :

            //Enable the parsing flag;
            packet_received = true;

            //Set the last byte to 0;
            *data_in = 0;

            //Complete;
            return;

        }


        //If the packet was corrupted, or empty;
        reset();


    } else {
        //If the packet hasn't been entirely received :

        //If data still can be inserted in the buffer :
        if (data_spaces) {

            //Append the get_read_adress char to data_in;
            *(data_in++) = read_char;

            //Increment the command size;
            command_size++;

            //Decrement the number of nb_spaces available;
            data_spaces--;

        } else {

            //Mark the current packet as corrupted;
            corrupted_packet = true;
        }
    }
}


/*
 * packet_received : this function returns the parsing flag;
 */

bool ClearText::parsing_ready() {
    return packet_received;
}


/*
 * get_data : this function return a pointer to the initial value of data_in buffer;
 */

char *ClearText::get_data() {

    //Reset the parsing flag;
    packet_received = false;

    //Reset the environment;
    reset();

    //Return the beginning of the input buffer;
    return data_in_0;

}


/*
 * reset : this function resets the parsing structure;
 */

void ClearText::reset() {

    //No data;
    command_size = 0;

    //Clear the corruption flag;
    corrupted_packet = false;

    //Maximum numbers of char nb_spaces;
    data_spaces = buffer_size;

    //data insertion at the origin;
    data_in = data_in_0;
}

#define m ClearText



