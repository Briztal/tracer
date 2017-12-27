/*
  EEPROM.cpp - Part of TRACER

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

#include <DataStructures/StringUtils.h>
#include <Interaction/Interaction.h>
#include "EEPROM.h"

//--------------------------------------------- Initialisation -----------------------------------------------------


void EEPROM::initialise_data() {

    //Delete the data tree;
    delete data_tree;

    //Instantiate a new data tree;
    data_tree = new EEPROMTree(new String("root"), nullptr);

    //Enable the tree modification;
    lock_flag = false;

}


//------------------------------------------- EEPROM Interaction ---------------------------------------------------

/*
 * write_tree : save the data tree in the EEPROM;
 */

void EEPROM::write_tree() {

    uint8_t index = 0;
}

void EEPROM::_write_tree() {

}


/*
 * read_tree : read the data tree from the EEPROM;
 */

void EEPROM::read_tree() {

}


/*
 *
 */

void EEPROM::hash_tree() {

}

//--------------------------------------------- Tree population -----------------------------------------------------


/*
 * add_branch : this function adds a new branch in the data tree, containing the provided data pointer;
 *
 *  The name string provided contains the name of the new tree that will be created;
 */

void EEPROM::add_branch(char *id_string, float *data_pointer) {

    //Create the required path if necessary;
    EEPROMTree *tree = createIfAbsent(id_string);

    //Update the data pointer;
    tree->setDataPointer(data_pointer);

}


/*
 * add_branch : this function appends the provided tree to the specified tree's children
 *
 *  The name string provided contains the name of the tree that will contain the child;
 */

void EEPROM::add_branch(char *id_string, EEPROMTree *child) {

    //Create the required path if necessary;
    EEPROMTree *tree = createIfAbsent(id_string);

    //Update the data pointer;
    tree->addChild(child);

}


/*
 * createIfAbsent : this function will create the required path in the data tree if it doesn't exist.
 *
 *  It returns the tree named ny the path;
 */
EEPROMTree *EEPROM::createIfAbsent(char *id_string) {


    //Initialise a pointer to the tree we currently examine children;
    EEPROMTree *current_tree = data_tree;

    //The parsing routine begins here;
    parse_word:

    //Remove extra spaces at the beginning of the string;
    id_string += StringUtils::lstrip(id_string, ' ');

    //Cache the beginning of the word;
    char *word_ptr = id_string;


    //Get the size of the next word;
    uint8_t word_size = StringUtils::count_until_char(id_string, ' ');

    //If the word is empty, stop;
    if (!word_size)
        return current_tree;

    //Update the id string;
    id_string += word_size;

    //Keep building if the current char is not an end char;
    bool keep_building = (*id_string != 0);

    //Nullify the end of the word;
    *id_string = '\0';

    //Get or create if absent the child corresponding to the word;
    current_tree = current_tree->createChildIfAbsent(word_ptr);

    //If the id string is not finished, more children are to create;
    if (keep_building) {

        //Parse another word;
        goto parse_word;

    }

    //return the tree corresponding to the specified path;
    return current_tree;

}


/*
 * lock_tree : this function disables any modification in the data tree;
 */

void EEPROM::lock_tree() {

    //Disable the tree modification;
    lock_flag = true;

}


void EEPROM::print_tree() {

    for (uint8_t i = 0; i < data_tree->getNbChildren(); i++) {

        std_out("child : "+String(*(data_tree->getChild(i)->getName())));

    }

    //std_out("SUUS");
    //std_out(data_tree->printTree());

}


EEPROMTree * EEPROM::search_tree_by_string(char *data_in) {

    //Initialise the current current_tree to the root;
    EEPROMTree *current_tree = data_tree;
    EEPROMTree *current_sub_tree;

    //char name_buffer[EEPROM_NAMES_MAX_LENGTH];

    uint8_t i;

    char *word;

    node_check:

    //remove extra spaces
    data_in += StringUtils::lstrip(data_in, ' ');

    //Get the word address;
    word = data_in;

    //Go to the next word;
    data_in += StringUtils::count_until_char(data_in, ' ');

    //Increment the next char if the string is not finished;
    if (*data_in) {
        *(data_in++) = 0;
    }

    //Cache the max number of children;
    uint8_t nb_children = current_tree->getNbChildren();

    //Check every sub_node
    for (i = 0; i < nb_children; i++) {

        //Update the current sub_tree;
        current_sub_tree = current_tree->getChild(i);

        //Cache the sub tree name;
        const char *c = (*current_sub_tree->getName()).c_str();

        //If the current word matches the current_tree's name
        if (!strcmp(c, word)) {

            //Re-initialise_hardware the current data
            current_tree = current_sub_tree;

            //if the new node is not a leaf, check sub nodes
            if (current_tree->getNbChildren()) {

                //check the new node
                goto node_check;

            } else {
                //If the sub_node is a leaf:

                //Return the current tree;
                return current_tree;

            }

        }

    }

    search_log(current_tree);

    return nullptr;

}


void EEPROM::search_log(EEPROMTree *tree) {

    //Log message
    String s = "Suggestions for " + *tree->getName() + " : ";

    //Cache the number of children;
    uint8_t nb_children = tree->getNbChildren();

    //Append the suggestion for every child tree;
    for (uint8_t i = 0; i < nb_children; i++) {
        s += *tree->getChild(i)->getName() + ", ";
    }

    std_out(s);

}


bool EEPROM::read_data_by_string(char *id_string, float *f) {

    //Get the required tree;
    EEPROMTree *tree = search_tree_by_string(id_string);

    //If the tree is not null
    if (tree) {
        *f = tree->getData();
        return true;
    } else {
        return false;
    }

}


void EEPROM::write_data_by_string(char *id_string, float value) {

    //Get the required tree;
    EEPROMTree *tree = search_tree_by_string(id_string);

    //If the tree is not null
    if (tree) {
        tree->setData(value);
    }

}


//------------------------------------- Static Declaration - definition -------------------------------------

//The lock flag;
bool EEPROM::lock_flag = false;

//The data tree;
EEPROMTree *EEPROM::data_tree = new EEPROMTree(new String("root"), nullptr);
