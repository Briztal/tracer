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
#include <Control/Controllers/_interface_data.h>
#include "GCodeTree.h"

#define GI GCodeInterpreter

class GCodeInterpreter {


public :

    //Hardware setup function;
    static void initialise_hardware();

    //Send the initialisation message
    static void init_message();

    //Data initialise_hardware function;
    static void initialise_data();




    //--------------------------------------Command Parsing----------------------------------


private :

    //Initialise the parsing.
    static void init_parsing();

    //Parse the GCode Command.
    static bool parse();

    //Analyse the GCode Command index, and schedule the associated command.
    static void schedule_command(char *command);

    //Schedule a GCodeInterfaceCommand function.
    static void schedule(task_state_t (*f)(char *));

    //Execute a parsed command;
    static task_state_t execute_command(void *data_pointer);

    //Finalise the execution of a command;
    static void confirm_command_execution(const interface_data_t *data);

    //The GCode Command tree;
    static GCodeTree *command_tree;


    //------------------------------------Standard functions-----------------------------

public :

    //System alias for echo;
    static void echo(const string_t msg);

    //System alias for a response (doesn't add //);
    static void respond(const String msg);

    //System alias for send_position;
    static void send_position(float*){}

};

#endif //CODE_GCodeExecuter_H
#endif