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

#include <Config/control_config.h>

#ifdef ENABLE_GCODE_INTERFACE

#ifndef GCODEINTERFACE
#define GCODEINTERFACE

#include <Control/Controllers/_controller_data.h>
#include <Control/Protocols/Protocol.h>
#include "GCodeTree.h"

class GCode {


public :

    //Send the initialisation message
    static void init_message();

    //Data initialisation function;
    static void initialise_data(Protocol *protocol);


    //--------------------------------------Command Parsing----------------------------------


public:

    //Parse the GCode Command.
    static bool parse(char *message);


private :

    //Analyse the GCode Command index, and schedule the associated command.
    static void schedule_command(const char *command_id, const char *arguments);

    //Execute a parsed command;
    static task_state_t execute_command(void *data_pointer);

    //Finalise the execution of a command;
    static void confirm_command_execution(const controller_data_t *data);

    //The GCode Command tree;
    static GCodeTree *command_tree;


    //------------------------------------ Output -----------------------------

public :

    //System alias for echo;
    static void log(const string_t msg);

    //System alias for echo;
    static void external_log(Protocol *protocol, const string_t msg);

    //System alias for send_position;
    static void send_position(float*){}

private:

    //System alias for a response (doesn't add //);
    static void respond(const String msg);



private:

    static Protocol *output_protocol;

};

#endif //CODE_GCodeExecuter_H
#endif