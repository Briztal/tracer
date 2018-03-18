//
// Created by root on 3/18/18.
//

#include <Interaction/Interaction.h>
#include <DataStructures/StringUtils.h>
#include "StringTreeLanguage.h"

/*
 * Constructor;
 */

StringTreeLanguage::StringTreeLanguage(uint8_t max_string_length, uint8_t max_children_nb) :
        commandTree(max_children_nb), max_children_nb(max_children_nb) {}


/*
 * decode : this function will read each word of the message string until a match (or unmatch) is found in the
 *  command tree;
 */

bool StringTreeLanguage::decode(const char *message) {

    //Display the received command
    std_out(tstring("\nTRACER > ") + message);

    //Initialise the current current_tree to the root;
    const StringTree *current_tree = &commandTree;


    //--------------------------Tree Iteration--------------------------

    //While the first letter of the next word is not the string termination :
    while(*(message += StringUtils::lstrip(message, ' '))) {

        //Cache the word's size;
        uint8_t word_size = StringUtils::count_until_char(message, ' ');

        //Create a string that will contain the word;
        string word(message, word_size);

        //Go to the end of the word (space or null termination);
        message += word_size;

        //Go to the next letter if not null
        message+=StringUtils::lstrip(message, ' ');

        //Get the child with the given label;
        StringTree *child_tree = current_tree->getChild(word);

        //If the current node didn't contain the required command :
        if (child_tree == nullptr) {

            //Log and fail;
            break;
        }

        //Update the current tree;
        current_tree = child_tree;

        //if the new node is a leaf, schedule its command;
        if (!current_tree->getChildrenNb()) {

            //If the new node is a leaf, we can schedule its command;
            schedule_command(this, current_tree->getNode(), message);

            //Complete;
            return true;

        }

    }

    //If no child was found, or if commad part was too short to point a leaf :

    //Log the curent tree's content;
    //TODO log_parsing_error(current_tree);

    //Fail
    return false;

}

void StringTreeLanguage::addCommand(const char *command_name, language_function) {

}
