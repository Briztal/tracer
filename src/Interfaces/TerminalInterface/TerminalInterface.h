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


#ifndef CODE_TERMINAL_INTERFACE
#define CODE_TERMINAL_INTERFACE

#include <config.h>

#ifdef ENABLE_TERMINAL_INTERFACE

#include <hardware_language_abstraction.h>
#include <DataStructures/ArgumentsContainer.h>
#include "Project/Config/terminal_interface_config.h"
#include <Interfaces/_interface_data.h>
#include "TerminalTree.h"

#define TI TerminalInterface

class TerminalInterface {


public :

    //--------------------------------------Standard Interface functions --------------------------------------

    //The hardware initialisation function;
    static void initialise_hardware();

    //The data initialisation function;
    static void initialise_data();

    //Function to read_integer the received data over the serial;
    static bool read_data();

    //Function to send a string over the serial;
    static void echo(const string_t msg);

    //Function to send the stepper position over the serial;
    static void send_position(float *) {}


    //--------------------------------------Data Storage--------------------------------------

private :

    //The command size;
    static unsigned char command_size;

    //The current number of available spaces in the data buffer;
    static uint8_t data_spaces;

    //A flag set if the current packet is corrupted (too long for the data buffer);
    static bool corrupted_packet;

    //The current address to store input data;
    static char *data_in;

    //The input data buffer's address;
    static char *const data_in_0;


    //--------------------------------------Command processing--------------------------------------

private:

    //Function to reset the command reception environment;
    static void reset();

    //Prepare the storage data for execution;
    static void prepare_execution();

    //Function to show a log message if the parsing failed;
    static void log_parsing_error(const TerminalTree *const log_node);

    //Function to parse and analyse the received command, and schedule the execution of execute_command;
    static void schedule_command();

    //This function will execute a TerminalCommand, after getting its arguments, and remove them after execution;
    static task_state_t execute_command(void *data_pointer);

    //Post execution function;
    static void confirm_command_execution(const interface_data_t *data);


    //--------------------------------------Tree data----------------------------------

private:

    //The command tree
    static const TerminalTree *command_tree;

};


#endif

#endif
