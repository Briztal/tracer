/*
  TerminalInterface.h - Part of TRACER

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

#include <config.h>

#ifdef ENABLE_TERMINAL_INTERFACE


#ifndef CODE_TERMINAL_INTERFACE
#define CODE_TERMINAL_INTERFACE

#include <hardware_language_abstraction.h>
#include "terminal_interface_config.h"


#define UI TerminalInterface


class TerminalInterface {


public :

    //The initialisation function.
    static void begin();


    //--------------------------------------Serial read --------------------------------------

public :

    //Function to read the received data over the serial.
    static void read_serial();


private :

    //Function to reset the command reception environment.
    static void reset();


    //The command size, and a saving variable.
    static unsigned char command_size;
    static unsigned char saved_command_size;

    //The input data buffer.
    static char *data_in;
    static char *const data_in_0;

    //--------------------------------------Command processing----------------------------------


private :

    //Function to parse and execute the recieved command.
    static void execute();

    //Function to show a log message if the parsing command didn't complete.
    static void fail_log();

    //Function to extract next word of the command.
    static unsigned char get_next_word();


private :

    //The word buffer.
    static char * word_buffer;
    static char *const word_buffer_0;


    //------------------------------------Standard functions-----------------------------

public :

    //Function to send a string over the serial.
    static void echo(const string_t msg);

    //Function to send the stepper position over the serial.
    static void send_position(float *) {}


};


#endif

#endif
