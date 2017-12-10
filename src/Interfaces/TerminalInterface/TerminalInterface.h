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
#include "TerminalNode.h"

#define TI TerminalInterface

class TerminalInterface {


public :

    //--------------------------------------Standard Interface functions --------------------------------------

    //The initialisation function.
    static void init();

    //Function to read_integer the received data over the serial.
    static bool read_data();

    //Function to send a string over the serial.
    static void echo(const string_t msg);

    //Function to send the stepper position over the serial.
    static void send_position(float *) {}




    //--------------------------------------Data Storage--------------------------------------

private :

    //The command size.
    static unsigned char command_size;

    //The current number of available spaces in the data bugger
    static uint8_t data_spaces;

    //A flag set if the current packet is corrupted (too long for the data buffer)
    static bool corrupted_packet;

    //The current address to store input data
    static char *data_in;

    //The input data buffer's address.
    static char *const data_in_0;


    //--------------------------------------Command processing--------------------------------------


private:

    //Function to reset the command reception environment.
    static void reset();

    //Prepare the storage data for execution
    static void prepare_execution();

    //Function to show a log message if the parsing failed.
    static void log_parsing_error(const TerminalNode *const log_node);

    //Function to parse and analyse the received command, and schedule the execution of execute_command.
    static void schedule_command();

    //This function will execute a TerminalCommand, after getting its arguments, and remove them after execution.
    static task_state_t execute_command(void *data_pointer);

    static void confirm_command_execution(const interface_data_t *data);


    //--------------------------------------Arguments Processing--------------------------------------

public:

    /*
     * A terminal interface command can accept an undefined number of arguments, in an argument_t sequence.
     *
     *  An argument_t sequence is a string structured like :
     *      -i_0 arg_0 -i_1 arg_1 ... -i_n arg_n
     *
     *  where all i_k are characters, and arg_k is a word (meaning anything). The character - is mandatory
     *      before an identifier, as arg_k can be none (empty string).
     *
     */

    //Parse the provided arguments, and save the data in the local buffer.
    static bool parse_arguments(char *current_position);

    //Get the pointer to the required argument_t
    static char *get_argument(char id);

    //Get a previously parsed argument_t if it exists
    static float get_argument_value(char id);

    //Verify that all arguments (defined by their arguments) have been provided (arguments is null terminated).
    static bool verify_all_identifiers_presence(const char *identifiers);

    //Verify that at least one argument_t (defined by their arguments) have been provided (arguments is null terminated).
    static bool verify_one_identifiers_presence(const char *identifiers);

    //verify that an argument_t identifier has be provided.
    static bool verify_identifier_presence(char id);


private:

    //The arguments sequences container
    static ArgumentsContainer arguments_storage;

    //Identifiers in a parsed argument_t sequence
    static argument_t *const arguments;

    //Number of arguments in a sequence
    static uint8_t nb_identifiers;

    //--------------------------------------Tree data----------------------------------

private:

    //The command tree
    static const TerminalNode *const command_tree;

    //The command tree summary, required to generate the tree.
    static String *tree_summary;


    //--------------------------------------Command processing--------------------------------------

    //Function to build the summary of the tree that will be used to generate the effective tree.
    static String *build_tree_summary();

    //Function to get the number of sub_nodes of a node in the tree.
    static uint8_t get_sub_nodes_nb(uint16_t command_index);

    //Function to generate the tree used to parse commands
    static TerminalNode *generate_tree();



};


#endif

#endif
