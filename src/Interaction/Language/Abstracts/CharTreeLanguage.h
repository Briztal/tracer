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
#include <DataStructures/Containers/Tree.h>

#ifndef TRACER_BYTETREELANGUAGE_H
#define TRACER_BYTETREELANGUAGE_H

#include <Interaction/Language/Language.h>

typedef Tree<char, language_function> CharTree;

class CharTreeLanguage : public virtual Language {

    //-------------------------------------- Initialisation ----------------------------------

public :

    //Default constructor;
    CharTreeLanguage(uint8_t max_children_nb);

    //Destructor;
    ~CharTreeLanguage() = default;


    //--------------------------------------Decoding ---------------------------------

public:

    //Parse the ByteTreeLanguage Command.
    bool decode(const char *message) final;


private :

    //Analyse the command index, and schedule the associated command.
    bool analyseCommand(const char *command_id, const char *arguments);

    //Finalise the execution of a command;
    //void confirm_command_execution(const controller_data_t *data);


    //------------------------------------ Tree Creation -----------------------------

public:

    //Add a null terminated command to the tree;
     void addCommand(const char *command_name, language_function);


protected:

    //The ByteTreeLanguage Command tree;
    CharTree commandTree;

    //The maximum number of children in our tree;
    uint8_t max_children_nb;


};

#endif //TRACER_BYTETREELANGUAGE_H
