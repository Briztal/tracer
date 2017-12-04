/*
  GCodeExecuter.h - Part of TRACER

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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <config.h>
#ifdef ENABLE_GCODE_INTERFACE

#ifndef GCODEINTERFACE
#define GCODEINTERFACE

#include <Project/Config/gcode_interface_config.h>
#include <hardware_language_abstraction.h>
#include <DataStructures/ArgumentsContainer.h>
#include <TaskScheduler/TaskScheduler.h>
#include <Interfaces/_interface_data.h>


#define GI GCodeInterface

class GCodeInterface {


private :


public :

    //Init function
    static void init();

    //--------------------------------------Serial Read Fields--------------------------------------

private :

    //The current command's size.
    static unsigned char command_size;

    //The current data pointer.
    static char *data_in;

    //The beginning of the data pointer.
    static char *const data_in_0;


    //--------------------------------------Parsing Fields--------------------------------------

public:

    /*
     * A Gcode Inteface command can accept an undefined number of arguments, in an argument sequence.
     *
     *  An argument sequence is a sequence of word, where the first letter is the identifier, and
     *      the rest of the word is an eventual value, example
     *
     *      X100 Y200 Q300 R2.0214
     *
     *  where all i_k are characters, and arg_k is a word (meaning anything). The character - is mandatory
     *      before an identifier, as arg_k can be none (empty string).
     *
     */

    //Parse the provided arguments, and save the data in the local buffer.
    static bool parse_arguments(char *args_current_position);

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


    //The arguments container.
    static ArgumentsContainer arguments_storage;

    //Identifiers in a parsed argument_t sequence
    static argument_t *const arguments;

    //Number of arguments in a sequence
    static uint8_t nb_identifiers;



    //--------------------------------------Command Parsing----------------------------------

public :
    //read data on the data_link
    static void read_data();


private :

    //Initialise the parsing.
    static void init_parsing();

    //Parse the GCode Command.
    static bool parse();

    //Reset the reading/parsing environment.
    static void reset();

    //Analyse the GCode Command index, and schedule the associated command.
    static void analyse_command(char *command, unsigned char command_size);

    //Schedule a GCodeInterfaceCommand function.
    static void schedule(task_state_t (*f)(char *));

    static task_state_t execute_command(void *data_pointer);



    //------------------------------------Standard functions-----------------------------

public :

    //System alias for echo
    static void echo(const string_t msg);

    //System alias for send_postion
    static void send_position(float*){}


};

#endif //CODE_GCodeExecuter_H
#endif