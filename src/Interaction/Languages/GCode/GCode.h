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

#include <Interaction/Languages/_language_data.h>
#include <Interaction/Protocols/Protocol.h>
#include <Interaction/Languages/Language.h>
#include "gcode_tree.h"


class GCode : Language {

    //-------------------------------------- Initialisation ----------------------------------

public :

    //Default constructor;
    GCode();

    //Destructor;
    ~GCode();

    //Send the initialisation message
    void init_message() final;


    //--------------------------------------Command Parsing---------------------------------

public:

    //Parse the GCode Command.
    bool parse(char *message) final;


private :

    //Analyse the GCode Command index, and schedule the associated command.
    void schedule_command(const char *command_id, const char *arguments);

    //Execute a parsed command;
    task_state_t execute_command(void *data_pointer);

    //Finalise the execution of a command;
    void confirm_command_execution(const controller_data_t *data);


    //------------------------------------ Tree Generation -----------------------------

    //Generate the gcode Tree, left to sub classes;
    virtual void generateTree();


    //------------------------------------ Output -----------------------------

public :

    //System alias for echo;
    void log(const char *msg);

    //System alias for echo;
    //void external_log(Protocol *protocol, const char *msg);


private:

    //System alias for a response (doesn't add //);
    void respond(const char *msg);


private:

    //The GCode Command tree;
    GCodetree commandTree;


};

#endif //CODE_GCodeExecuter_H
#endif