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

#include <Config/control_config.h>

#ifdef ENABLE_TERMINAL_INTERFACE

#include "TerminalTreeGenerator.h"
#include <Project/InterfaceCommands/TerminalCommands.h>

//--------------------------------------------------Tree generation-----------------------------------------------------

/*
 * generate_sub_nodes_string : this function generates a string, containing the summary of the tree creation :
 *  - 0 means go_upper
 *  - 1 means create_leaf
 *  - 2 means go_lower
 *
 *  This string is used to determine the number of children of a particular node.
 */

void TerminalTreeGenerator::build_tree_summary(String *tree_summary) {

    //Set the initial go_lower
    tree_summary->append((char) 2);

#define GO_LOWER(...) tree_summary->append((char)2);

#define GO_UPPER(...) tree_summary->append((char)0);

#define CREATE_LEAF(...) tree_summary->append((char)1);

#include "Config/controller_terminal_config.h"

#undef GO_UPPER

#undef GO_LOWER

#undef CREATE_LEAF


}


/*
 * generate_tree : this function generates the TerminalTree that will be used to parse commands.
 */

TerminalTree *TerminalTreeGenerator::generate_tree() {

    //Local string
    String s;

    //The pointer to pass;
    String *tree_summary = &s;

    //Create the tree summary;
    build_tree_summary(tree_summary);

    //TODO WAT ?
    uint16_t command_counter = 0;

    //Get the number of sons of root.
    uint8_t root_sons_nb = get_sub_nodes_nb(tree_summary,command_counter++);

    //Create the root tree.
    TerminalTree *root = new TerminalTree(new String("root"), root_sons_nb, new String("root"), nullptr);

    //Initialise the current tree and the history.
    TerminalTree *current_tree = root;
    TerminalTree *tree_history[TERMINAL_MAX_DEPTH];

    //Initialise the indices history
    uint8_t indices_history[TERMINAL_MAX_DEPTH];
    uint8_t current_index = 0;
    uint8_t depth = 0;

    uint8_t tmp_nb;

    bool tree_flag = false;

    /*
     * Procedure for a go_lower :
     *  - save the index
     *  - save the current node
     *  - get the children number for the current node;
     *  - create a new current node
     *  - increment the depth;
     */

#define GO_LOWER(name, desc)\
    indices_history[depth] = current_index;\
    current_index = 0;\
    tree_history[depth] = current_tree;\
    tmp_nb = get_sub_nodes_nb(tree_summary, command_counter++);\
    current_tree = new TerminalTree(new String(#name), tmp_nb, new String(#desc), nullptr);\
    depth++;


    /*
     * Procedure for a go_upper :
     *  - decrement the depth;
     *  - restore the previous index;
     *  - use it to add the current node to the previous node'tree_summary sons;
     *  - increment the index;
     *  - restore the previous node;
     *  - increment the command counter.
     */

#define GO_UPPER()\
    depth--;\
    current_index = indices_history[depth];\
    tree_history[depth]->set_child(current_index++, current_tree, &tree_flag);\
    current_tree = tree_history[depth];\
    command_counter++;


    /*
     * Procedure for a leaf creation :
     *  - create a leaf node;
     *  - add it to the current node;
     *  - increment the index;
     *  - increment the command counter.
     */

#define CREATE_LEAF(name, function, desc)\
    current_tree->set_child(current_index++, new TerminalTree(new String(#name), 0, new String(#desc), TerminalCommands::function), &tree_flag);\
    command_counter++;

#include "Config/controller_terminal_config.h"

#undef GO_LOWER

#undef GO_UPPER

#undef CREATE_LEAF

    return root;
}


/*
 * get_sub_nodes_nb : this function determines the number of direct children of a particular indice in the children
 *  string.
 */

uint8_t TerminalTreeGenerator::get_sub_nodes_nb(String *tree_summary, uint16_t command_index) {

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

#endif