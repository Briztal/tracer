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

#include "LCDInterface.h"
#include <hardware_language_abstraction.h>


/*
 * begin : this function initialises the interface.
 *
 *  It starts by initialising the physical link, and then sets up interrupts, for the human control of the display.
 *
 */

void EmbeddedInterface::begin() {

    lcd_interface_link_t::begin();

    //TODO SETUP INTERRUPTS

}


/*
 * update_data : this function updates the control screen data.
 *
 */

void EmbeddedInterface::update_data() {

    //TODO : manual data import
}


/*
 * refresh : this function updates the control screen.
 */

void EmbeddedInterface::refresh() {

    //TODO : Customisable control screen

}


/*
 * incr_selection : this function increases the selection for the current node.
 *
 */

void EmbeddedInterface::incr_selection() {

    //Increase if the current is not the max.
    if (current_selection!=max_current_selection)
        current_selection++;

}


/*
 * decr_selection : this function decreases the selection for the current node.
 *
 */

void EmbeddedInterface::decr_selection() {

    //decrease if the current is not zero
    if (current_selection)
        current_selection--;

}


/*
 * select_node : this function validates the selection on the current node.
 *
 *  It determines the next node environment.
 *
 */

void EmbeddedInterface::select_node() {

    //Save the node and the selection, and increase the depth.
    node_history[depth] = current_node;
    selection_history[depth++] = current_selection;

    //update the node and the selection variables
    current_node = current_node->sub_nodes[current_selection];
    current_selection = 0;
    max_current_selection = current_node->sub_nodes_nb;

}


/*
 * go_back : this function validates the selection on the current node.
 *
 *  It determines the next node environment.
 *
 */

void EmbeddedInterface::go_back() {

    if (!depth)
        return;
    //update the node and the selection variables
    current_node = node_history[--depth];
    current_selection = selection_history[depth];
    max_current_selection = current_node->sub_nodes_nb;

}


/*
 * select_root : go back to the control screen
 *
 */

void EmbeddedInterface::select_root() {

    if (!depth)
        return;

    //zero the depth
    depth = 0;

    //update the node and the selection variables
    current_node = node_history[0];
    current_selection = 0;
    max_current_selection = current_node->sub_nodes_nb;

}


}

