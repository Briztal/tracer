/*
  TerminalTreeGenerator.h - Part of TRACER

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


#ifndef TRACER_TERMINALTREEGENERATOR_H
#define TRACER_TERMINALTREEGENERATOR_H


#include <Config/control_config.h>

#ifdef ENABLE_TERMINAL_INTERFACE

#include "hardware_language_abstraction.h"
#include "TerminalTree.h"

class TerminalTreeGenerator {

    //Function to build the summary of the tree that will be used to generate the effective tree.
    static String *build_tree_summary(String *tree_summary);

    //Function to get the number of children of a node in the tree.
    static uint8_t get_sub_nodes_nb(String *tree_summary, uint16_t command_index);


public:

    //Function to generate the tree used to parse commands
    static TerminalTree *generate_tree();

};


#endif //TRACER_TERMINALTREEGENERATOR_H

#endif