/*
  EmbeddedInterface.h - Part of TRACER

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

#ifdef ENABLE_LCD_INTERFACE

#ifndef TRACER_EMBBEDINTERFCE_H
#define TRACER_EMBBEDINTERFCE_H

#include <stdint.h>
#include <hardware_language_abstraction.h>
#include "LCDNode.h"

class EmbeddedInterface {

public:

    //method to init the serial and setup control interrupts.
    static void begin();

    //refresh the screen.
    static void refresh();


    //---------------------------------------------------Custom Data----------------------------------------------------

public:

    //update the data of the control_screen.
    static void update_data();


private:

    //The stepper coordinates
    static float *hl_coordinates;

    //The temperatures coordinates
    static float *temps;


    //--------------------------------------------------Tree selection--------------------------------------------------

private:

    //increase the selection for the current menu.
    static void incr_selection();

    //decrease the selection for the current menu.
    static void decr_selection();

    //Go to the selected node.
    static void select_node();

    //Go back to the parent node.
    static void go_back();

    //Go back to the control screen.
    static void select_root();



    //----------------------------------------------------Tree data-----------------------------------------------------

private:


    static LCDNode **node_history;
    static LCDNode *current_node;

    //The current selection, its history, and its upper bound
    static uint8_t *selection_history;
    static uint8_t current_selection;
    static uint8_t max_current_selection;

    static uint8_t depth;


    /*

    //tree indices array
    static uint8_t *tree_indices;

    //tree indices array origin.
    static uint8_t *const tree_indices_0;


    static string_t *sub_nodes_names;

    static uint8_t sub_nodes_nb;

     */

};


#endif //TRACER_EMBBEDINTERFCE_H

#endif