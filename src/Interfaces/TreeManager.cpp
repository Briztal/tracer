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


#include "TreeManager.h"

/*
 * Constructor : initialises all simple attributes, and builds the choice tree.
 *
 */

TreeManager::TreeManager(Node *default_tree) : current_tree(default_tree), default_tree(default_tree) {


    //building the choice tree
    choice_tree = new Node(new String(), 3, new String(), new String(), 0);

    //Assigning the sub nodes
    for (uint8_t i = 0; i < 3; i++) {
        choice_tree->sub_nodes[0] = new Node(new String(), 3, new String(), new String(), 0);
    }

}

/*
 * get_current_tree : getter for the current tree;
 *
 */

const Node *TreeManager::get_current_tree() {
    return current_tree;
}


/*
 * select_choice_tree : this function sets the default tree as the active tree.
 *
 */

void TreeManager::select_default_tree() {

    current_tree = default_tree;

}


/*
 * select_choice_tree : this function sets the choice tree as the active tree.
 *
 */

void TreeManager::select_choice_tree() {

}


/*
 * edit_choice_labels : this function sets the name, the description and the args of the choice node, after
 *  deleting old ones.
 *
 */

void TreeManager::edit_choice_tree_labels(uint8_t choice, String *name, String *desc, String *args) {

    if (choice >= 3)
        return;

    //Cache var
    Node *n = choice_tree->sub_nodes[choice];

    //Clean the old Strings
    delete n->name;
    delete n->desc_log;
    delete n->args_log;

    //Update the new ones
    n->name = name;
    n->desc_log = desc;
    n->args_log = args;

}


/*
 * edit_choice_tree_functions : this function updates the choice_tree functions, for the provided ones.
 *
 */

void TreeManager::edit_choice_tree_functions(bool (*f_next)(char *, uint8_t), bool (*f_back)(char *, uint8_t),
                                             bool (*f_abort)(char *, uint8_t)) {

    choice_tree[0].function = f_next;
    choice_tree[1].function = f_back;
    choice_tree[2].function = f_abort;

}


/*
 * get_sub_nodes_nb : this function determines the number of direct sub_nodes of a particular indice in the sub_nodes
 *  string.
 *
 */

uint8_t TreeManager::get_sub_nodes_nb(uint16_t command_index, String *tree_summary) {

    uint16_t max = (uint16_t) tree_summary->length();

    //If the command index is below the command nb, return 0 for safety
    if (command_index > max) return 0;

    //If the first command is a create_leaf of go_upper, 0 sons.
    char v = tree_summary->charAt(command_index++);
    if (v == (char) 0 || v == (char) 1)
        return 0;

    //If not, we will count to the next go_upper (0);

    uint8_t sons_nb = 0;
    uint8_t depth = 0;

    v = tree_summary->charAt(command_index++);

    //We stop counting when we detect a zero at depth zero, or when the all command have been listed.
    // -> keep while the depth is not zero or the value is not zero, and command are still to be processed.
    while (((depth || v)) && (command_index != max)) {
        //correct the depth if we go upper of lower
        if (v == 0) {
            depth--;
        } else if (v == (char) 1) {
            if (!depth) sons_nb++;
        } else if (v == (char) 2) {
            if (!depth) sons_nb++;
            depth++;
        }
        v = tree_summary->charAt(command_index++);
    }

    return sons_nb;

}

