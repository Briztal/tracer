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

#include "CharTreeLanguage.h"


//----------------------------------------------------Initialisation----------------------------------------------------

/*
 * Default constructor : this function resets initialises all data of the class
 */

CharTreeLanguage::CharTreeLanguage(uint8_t max_children_nb) :
        max_children_nb(max_children_nb), commandTree(max_children_nb) {}


//---------------------------------------------------------Parse--------------------------------------------------------


/*
 * parse : this function parses the received ByteTreeLanguage Command.
 *
 *  It is called when a ByteTreeLanguage command has been entirely received
 */

bool CharTreeLanguage::decode(const char *message) {

    //Remove extra spaces;
    message += cstring::lstrip(message, ' ');

    //Save the pointer to the first char of the command id.
    const char *command_id = message;

    //Get the command id length;
    uint8_t command_id_length = cstring::count_until_char(message, ' ');

    //Abort if the first word was a space.
    if (!command_id_length)
        return false;

    //Update data_in
    message += command_id_length;

    //Analyse the command id.
    return analyseCommand(command_id, message);

}



/*
 * parse : this function analyses the packet received, and parses the command part.
 *
 *  When it has found the command identified by this string, it caches a pointer to the beginning of the
 *      argument part, and tries to find a function matching the command in the tree;
 */

bool CharTreeLanguage::analyseCommand(const char *command_id, const char *arguments) {

    //Initialise the current current_tree to the root;
    CharTree *current_tree = &commandTree;

    //The current char in the command_char;
    char current_char;

    //While we are not at the end of the command;
    while ((current_char = *(command_id++))) {

        //Get the child with the correct label;
        CharTree *child = current_tree->getChild(current_char);

        //If no child matches the current_char, wrong command;
        if (child == nullptr) {
            return false;
        }

        //Update the current node and the current nb_children array.
        current_tree = child;

        //if the new node is a leaf, schedule its command;
        if (!current_tree->getChildrenNb()) {

            //If the new node is a leaf, we can schedule its command;
            schedule_command(this, *current_tree->getNode(), arguments);

            //Complete;
            return true;
        }

        //If the new node is not single, let's check its children, during the next iteration;
    }

    //If command didn't lead to a leaf, fail;
    return false;

}


/*
 * addCommand : adds a command defined by its name and its function to the tree;
 */

void CharTreeLanguage::addCommand(const char *command_name, language_function command_function) {

    //In this function, we will have to search in the tree, for the given command name;

    //First, start the current tree;
    CharTree *current_tree = &commandTree;

    char current_char;

    //While we are not at the end of the command;
    while ((current_char = *(command_name++))) {

        //Get the child with the correct label;
        CharTree *child = current_tree->getChild(current_char);

        //If the child doesn't exist :
        if (child  == nullptr) {

            //Create the child;
            child = new CharTree(current_char, (language_function) nullptr, max_children_nb);

            //Add it to the current tree;
            current_tree->addChild(child);

        }

        //Now that the child exists, set it as the current tree;
        current_tree = child;

    }

    //Now, we are sure that current tree corresponds to the command name;

    //If its function is not nullptr (command reassignment):
    if (current_tree->getNode() != nullptr) {

        //Log;
        std_out("Warning, in CharTreeLanguage::addCommand : reassignment of command;");

    }

    //Update the function of the current tree to the required function;
    current_tree->setNode(command_function);

}
