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
#include "TerminalNode.h"


#define UI TerminalInterface


//A simple struct that will be used in the arguments parsing.
typedef struct id_to_index_t {
    char identifier;
    uint8_t index;
};

class TerminalInterface {


public :

    //--------------------------------------Standard Interface functions --------------------------------------


    //The initialisation function.
    static void init();

    //Function to read_integer the received data over the serial.
    static void read_data();

    //Function to send a string over the serial.
    static void echo(const string_t msg);

    //Function to send the stepper position over the serial.
    static void send_position(float *) {}


    //--------------------------------------Arguments Processing--------------------------------------

    /*
     * A terminal interface command can accept an undefined number of arguments, in an argument sequence.
     *
     *  An argument sequence is a string structured like :
     *      -i_0 arg_0 -i_1 arg_1 ... -i_n arg_n
     *
     *  where all i_k are characters, and arg_k is a word (meaning anything). The character - is mandatory
     *      before an identifier, as arg_k can be none (empty string).
     *
     */

    //Parse the provided arguments, and save the data in the local buffer.
    static bool parse_arguments(char *arguments_sequence);

    //Get a previously parsed argument if it exists
    static float get_argument(char id);

    //Verify that all arguments (defined by their identifiers) have been provided (identifiers is null terminated).
    static bool verify_identifiers_presence(const char *identifiers);


private:

    //The arguments sequences container
    static ArgumentsContainer arguments_sequences_storage;

    //Identifiers in a parsed argument sequence
    static id_to_index_t *const identfiers;

    //Number of arguments in a sequence
    static uint8_t nb_identifiers;

    //Arguments in the parsed sequence.
    static ArgumentsContainer arguments_storage;





    //--------------------------------------data storage--------------------------------------

private :

    //The command size.
    static unsigned char command_size;

    //The current address to store input data
    static char *data_in;

    //The input data buffer's address.
    static char *const data_in_0;

    //Function to reset the command reception environment.
    static void reset();



private :

    static void prepare_execution();


    //--------------------------------------Tree data----------------------------------

private:

    //The command tree
    static TerminalNode *command_tree;

    //The command tree summary, required to generate the tree.
    static String *tree_summary;


    //--------------------------------------Command processing--------------------------------------

    //Function to parse and analyse the received command
    static void execute();


    //--------------------------------------Command processing--------------------------------------

    //Function to build the summary of the tree that will be used to generate the effective tree.
    static String *build_tree_summary();

    //Function to get the number of sub_nodes of a node in the tree.
    static uint8_t get_sub_nodes_nb(uint16_t command_index);

    //Function to generate the tree used to parse commands
    static TerminalNode *generate_tree();


    //---------------------------------Functions called by TerminalInterfaceCommands------------------------------

public :

    //Get a particular argument in the storage
    static char *get_arguments(uint8_t task_index);

    //Mark a task as executed
    static void validate_task(uint8_t task_index);

    //Function to show a log message if the execution failed.
    static void log_tree_style(TerminalNode *log_node, bool log_args);


    static bool verify_identifier_presence(char id);
};


#endif

#endif
