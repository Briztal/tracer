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

#define GI GCodeInterface

//TODO PARSE DURING TASK EXECUTION ?

/*
 * We will declare a structure to contain parameters and their flags.
 *
 */

typedef struct gcode_arguments {

    //Parameters.
    float parameters[NB_PARAMETERS];

    //Parameter flags.
    bool parameters_flags[NB_PARAMETERS];

} gcode_arguments;


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

private :

    //The arguments container.
    static ArgumentsContainer arguments_storage;



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
    static void schedule(task_state_t (*f)(void *));


    //--------------------------------------Parameters----------------------------------

public:

    //Parse parameters to
    static bool parse_parameters(char *parameter_buffer, uint8_t parameters_size);


private :

    //The current arguments.
    static gcode_arguments args;

    //The current parameters size.
    static unsigned char parameters_size;

    //The current parameter pointer.
    static char *parameters_ptr;

    //get the next parameter in the GCode Command.
    static bool get_parameter(char *command, float *value);

    //Process the data given by the function behind.
    static bool process_parameter(char index, float value);


    //------------------------------------Standard functions-----------------------------

public :

    //System alias for echo
    static void echo(const string_t msg);

    //System alias for send_postion
    static void send_position(float*){}



};

#endif //CODE_GCodeExecuter_H
#endif