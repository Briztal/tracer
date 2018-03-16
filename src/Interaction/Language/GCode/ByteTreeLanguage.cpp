/*
  GCodeInterface.cpp - Part of TRACER

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

#include "ByteTreeLanguage.h"
#include <Kernel/TaskScheduler/TaskScheduler.h>
#include "GCodeArguments.h"
#include <DataStructures/StringUtils.h>
#include <Interaction/Interaction.h>


//----------------------------------------------------Initialisation----------------------------------------------------

/*
 * Default constructor : this function resets initialises all data of the class
 */

ByteTreeLanguage::ByteTreeLanguage() : commandTree() {

    //Arguments initialisation;
    GCodeArguments::clear();

    //Create a new command tree;
    generateTree();

}


//---------------------------------------------------------Parse--------------------------------------------------------


/*
 * parse : this function parses the received ByteTreeLanguage Command.
 *
 *  It is called when a ByteTreeLanguage command has been entirely received
 */

bool ByteTreeLanguage::decode(const char *message) {

    //Remove extra spaces;
    message += StringUtils::lstrip(message, ' ');

    //Save the pointer to the first char of the command id.
    const char *command_id = message;

    //Get the command id length;
    uint8_t command_id_length = StringUtils::count_until_char(message, ' ');

    //Abort if the first word was a space.
    if (!command_id_length)
        return false;

    //Update data_in
    message += command_id_length;

    //Analyse the command id.
    analyse(command_id, message);

    //Success
    return true;

}


/*
 * parse : this function analyses the packet received, and parses the command part.
 *
 *  When it has found the command identified by this string, it caches a pointer to the beginning of the
 *      argument part, and tries to find a function matching the command in the tree;
 */

void ByteTreeLanguage::analyse(const char *command_id, const char *arguments) {

    //Initialise the current current_tree to the root;
    const GCodeTree *current_tree = &commandTree;

    //The current depth
    uint8_t depth = 0;

    //Must declare the int before the jump label.
    uint8_t i;

    //A cache for the child of the current node we are focused on.
    const GCodeTree *current_child;

    //The current char in the command_char;
    char command_char;
    //--------------------------Tree Iteration--------------------------

    //As the algorithm involves two for - while loops, we must use a goto label to restart with a new node.
    node_check:

    //Get the command_char identifier, and increment the depth;
    command_char = command_id[depth++];


    //We now must compare each child of the current node, and compare its identifier with the read_data command_char identifier.

    //Check every sub_node
    for (i = 0; i < current_tree->getChildrenNb(); i++) {

        //We now need to get the child.

        //Update the current sub_node
        current_child = current_tree->getChild(i);

        const char tree_id = current_child->getLabel();

        //If the current command_identifier matches the current_tree's name
        if (command_char == tree_id) {

            //A matching sub_node has been found. It can be a single
            //Update the current node and the current nb_children array.
            current_tree = current_child;

            //if the new node is not a leaf, check sub nodes
            if (current_tree->getChildrenNb()) {

                //Restart the process with the new node
                goto node_check;

            } else {

                //If the new node is a leaf, we can schedule its command;
                schedule_command(current_tree->getNode(), arguments);

            }

        }

    }

}
