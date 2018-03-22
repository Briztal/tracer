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
        commandTree("root", nullptr, max_children_nb), max_children_nb(max_children_nb) {}


void print_tree(const StringTree *tree) {

    std_out(tree->getLabel() + " " + tree->getChildrenNb());

    const uint8_t size = tree->getChildrenNb();

    for (uint8_t child_pointer = size; child_pointer--;) {

        std_out("child : " + tree->getChild(child_pointer)->getLabel());

    }
}

/*
 * decode : this function will read each word of the message string until a match (or unmatch) is found in the
 *  command tree;
 */

bool StringTreeLanguage::decode(const char *message) {

    Serial.println("DECODING");

    //Display the received command
    std_out(tstring("\nTRACER > ") + message);

    //Initialise the current current_tree to the root;
    const StringTree *current_tree = &commandTree;

    print_tree(current_tree);


    //--------------------------Tree Iteration--------------------------

    //While the first letter of the next word is not the string termination :
    while (*(message += StringUtils::lstrip(message, ' '))) {

        //Cache the word's size;
        uint8_t word_size = StringUtils::count_until_char(message, ' ');

        //Create a string that will contain the word;
        string message_word(message, word_size);

        //Go to the end of the word (space or null termination);
        message += word_size;

        //Go to the next letter if not null
        message += StringUtils::lstrip(message, ' ');

        //Get the child with the given label;
        StringTree *child_tree = current_tree->getChild(message_word);

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

void StringTreeLanguage::addCommand(const char *command_name, language_function command_function) {

    TaskScheduler::setDefaultCommunicationPipe();

    delay(1000);

    //Initialise the current current_tree to the root;
    StringTree *current_tree = &commandTree;

    //print_tree(current_tree);

    Serial.println(command_name);

    //--------------------------Tree Iteration--------------------------

    //While the first letter of the next word is not the string termination :
    while (*(command_name += StringUtils::lstrip(command_name, ' '))) {

        //Cache the word's size;
        uint8_t word_size = StringUtils::count_until_char(command_name, ' ');

        std_out("UHJO");

        return;

        Serial.println("size : "+String(word_size));

        //Create a string that will contain the word;
        string message_word(command_name, word_size);

        Serial.println(message_word.data());

        //Go to the end of the word (space or null termination);
        command_name += word_size;

        Serial.println("INCR");

        //Go to the next letter if not null
        command_name += StringUtils::lstrip(command_name, ' ');

        Serial.println("Searching");

        //Get the child with the given label;
        StringTree *child_tree = current_tree->getChild(message_word);

        //If the current node didn't contain the required command :
        if (child_tree == nullptr) {

            Serial.println("NULLPTR");

            //Create the child;
            child_tree = new StringTree(message_word, (language_function) nullptr, max_children_nb);

            //Add it to the current tree;
            current_tree->addChild(child_tree);

            Serial.println("CREATED");
        }

        //Update the current tree;
        current_tree = child_tree;

    }

    //If its function is not nullptr (command reassignment):
    if (current_tree->getNode() != nullptr) {

        //Log;
        std_out("Warning, in CharTreeLanguage::addCommand : reassignment of command;");

    }

    //Update the function of the current tree to the required function;
    current_tree->setNode(command_function);


}
