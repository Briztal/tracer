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
#include <DataStructures/ArgumentsContainer.h>
#include "Project/Config/terminal_interface_config.h"
#include "DataStructures/Node.h"


#define UI TerminalInterface


class TerminalInterface {


public :

    //The initialisation function.
    static void init();


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


    static ArgumentsContainer arguments_storage;



private :

    static void prepare_execution();


    //--------------------------------------Command processing with tree style----------------------------------

private:

    static Node *command_tree;

    static String *tree_summary;

    //Function to generate the tree used to parse commands
    static Node *generate_tree();

    static uint8_t get_sub_nodes_nb(uint16_t command_index);

    static String *build_tree_summary();

    //Function to parse and execute the received command
    static void execute_tree_style();


    //---------------------------------Functions called by TerminalInterfaceCommands------------------------------

public :

    //Mark a task as executed
    static char *get_arguments(uint8_t task_index, uint8_t *size);

    //Mark a task as executed
    static void validate_task(uint8_t task_index);

    //Function to show a log message if the execution failed.
    static void log_tree_style(Node *log_node, bool log_args);

    //--------------------------------------Words extraction----------------------------------

private :

    //Function to extract next word of the command.
    static unsigned char get_next_word();

    //The word buffer.
    static char *word_buffer;
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
