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

#include <Interaction/Language/ArgumentLanguage.h>
#include "gcode_tree.h"


class ByteTreeLanguage : ArgumentLanguage {

    //-------------------------------------- Initialisation ----------------------------------

public :

    //Default constructor;
    ByteTreeLanguage();

    //Destructor;
    ~ByteTreeLanguage() = default;


    //--------------------------------------Decoding ---------------------------------

public:

    //Parse the ByteTreeLanguage Command.
    bool decode(const char *message) final;


private :

    //Analyse the command index, and schedule the associated command.
    void analyse(const char *command_id, const char *arguments);

    //Finalise the execution of a command;
    //void confirm_command_execution(const controller_data_t *data);


    //--------------------------------------Encoding ---------------------------------

public:

    //Parse the ByteTreeLanguage Command.
    void encode(tstring &, uint8_t type) final;


//private:

    //Finalise the execution of a command;
    //void confirm_command_execution(const controller_data_t *data);


    //------------------------------------ Tree Generation -----------------------------

    //Generate the gcode Tree, left to sub classes;
    virtual void generateTree();


private:

    //The ByteTreeLanguage Command tree;
    GCodeTree commandTree;


};

#endif //CODE_GCodeExecuter_H
#endif