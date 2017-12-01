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

#ifndef TRACER_TREEMANAGER_H
#define TRACER_TREEMANAGER_H


#include <Interfaces/TerminalInterface/TerminalNode.h>

class TreeManager {

    //---------------------------------------------------Constructor----------------------------------------------------

public:

    TreeManager(TerminalNode *default_tree);


    //---------------------------------------------------Constructor----------------------------------------------------

public:

    //Method to reset the current tree as the default tree;
    void select_default_tree();

    //Method to select the choice tree;
    void select_choice_tree();

    //Method to edit the labels of the choice tree
    void edit_choice_tree_labels(uint8_t choice, String *name, String *desc, String *args);

    //Method to edit the functions of the choice tree.
    void edit_choice_tree_functions(task_state_t (*f_next)(char *), task_state_t (*f_back)(char *), task_state_t (*f_abort)(char *));

    //----------------------------------------------------Tree data-----------------------------------------------------

public:

    const TerminalNode* get_current_tree();

private:

    TerminalNode *default_tree;

    TerminalNode *current_tree;

    TerminalNode *choice_tree;


    //-------------------------------------------------Tree generation--------------------------------------------------

public :

    //Function to get a node's number of children, according to the tree summary.
    static uint8_t get_sub_nodes_nb(uint16_t command_index, String *tree_summary);
};


#endif //TRACER_TREEMANAGER_H
