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
#include "GCodeTree.h"

#define GI GCodeInterface

class GCodeInterface {


public :

    //Hardware setup function;
    static void initialise_hardware();

    //Data initialise_hardware function;
    static void initialise_data();

    //--------------------------------------Serial Read Fields--------------------------------------

private :

    //The current command's size;
    static uint8_t command_size;

    //The current number of available spaces in the data buffer;
    static uint8_t data_spaces;

    //A flag set if the current packet is corrupted (too long for the data buffer);
    static bool corrupted_packet;

    //The current data pointer, points to a case in the data buffer;
    static char *data_in;

    //The beginning of the data buffer;
    static char *const data_in_0;


    //--------------------------------------Command Parsing----------------------------------

public :
    //read data on the data_link
    static bool read_data();


private :

    //Initialise the parsing.
    static void init_parsing();

    //Parse the GCode Command.
    static bool parse();

    //Reset the reading/parsing environment.
    static void reset();

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

    //System alias for echo
    static void echo(const string_t msg);

    //System alias for send_postion
    static void send_position(float*){}

};

#endif //CODE_GCodeExecuter_H
#endif