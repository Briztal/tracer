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


/*
 * read_data : this function reads and processes data.
 *
 *  It reads data on the link layer, saves it, and eventually processes it.
 */

bool ClearText::read_data() {


    //Don't process any data if no space is available in the argument_t sequence container
    if (!GCodeArguments::available_spaces()) {

        //Return true if the data_link buffer is not empty.
        return (bool) gcode_interface_link_t::available();

    }

    while (gcode_interface_link_t::available()) {

        //Read the data link
        char read_char = gcode_interface_link_t::read();

        //If the recieved char is a line feed or a working_extruder return
        if ((read_char == 10) || (read_char == 13)) {

            //If a non empty uncorrupted packet has effectively been received
            if (command_size && !corrupted_packet) {

                //Parse the GCode
                parse();

                //Reset the data_in
                reset();

                //Return true if the data_link buffer is not empty.
                return (bool) gcode_interface_link_t::available();

            }

            //If the received packet was too long for the input buffer :
            if (corrupted_packet) {

                //Display an error message
                GI::echo("WARNING in Terminal::read_data : the received packet was too long for "
                                 "the input buffer. Please check your terminal_interface_config.h");


            }

            //If the packet was corrupted, or empty
            reset();


        } else {
            //If the packet hasn't been entirely received

            //If data still can be inserted in the buffer
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

    //No more data available;
    return false;
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
    data_spaces = GCODE_MAX_SIZE;

    //data insertion at the origin;
    data_in = data_in_0;
}


#define m ClearText

//The command size
unsigned char m::command_size;

//The current number of available nb_spaces in the data bugger
uint8_t m::data_spaces = GCODE_MAX_SIZE;

//A flag set if the current packet is corrupted (too long for the data buffer)
bool m::corrupted_packet = false;

//Data pointers
char tdatain_gcode[GCODE_MAX_SIZE];
char *m::data_in = tdatain_gcode;
char *const m::data_in_0 = tdatain_gcode;


