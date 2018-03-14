/*
  EEPROMNode.cpp - Part of TRACER

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

#include <Interaction/Interaction.h>
#include "EEPROMTree.h"


/*
 * Constructor : this function initialises all fields;
 */

EEPROMTree::EEPROMTree(string *name, float *const data, float default_value) : name(name), data(data),
                                                                                 default_value(default_value) {

    //Initialise the children pointer to nullptr;
    children = (EEPROMTree **) malloc(0);

    //Initialise the number of children to zero;
    nb_children = 0;

}


/*
 * Destructor : this function removes all dynamic fields;
 */

EEPROMTree::~EEPROMTree() {

    //Delete each child;
    for (uint8_t child = 0; child < nb_children; child++) {

        delete children[child];

    }

    //Free the child container;
    free(children);

    //Delete the name, that is dynamic;
    delete name;

}


/*
 * getNbChildren : this function returns the current number of children of the tree;
 */

const uint8_t EEPROMTree::getNbChildren() {
    return nb_children;
}


/*
 * getName ; this function returns the name of the tree;
 */

const string *EEPROMTree::getName() {

    return name;
}


/*
 * addChild : this function adds a child to the children array, using realloc;
 */

void EEPROMTree::addChild(char *child_name) {

    //If a space is successfully added in the children array;
    if (_addChild()) {

        //Save the new child;
        children[nb_children - 1] = new EEPROMTree(new string(child_name), nullptr, 0);

    }
}


/*
 * addChild : this function adds a child to the children array, using realloc;
 */

void EEPROMTree::addChild(EEPROMTree *child) {

    //If a space is successfully added in the children array;
    if (_addChild()) {


        //Save the new child;
        children[nb_children - 1] = child;

    }

}


/*
 * addChild : this function adds a child to the children array, using realloc;
 */

bool EEPROMTree::_addChild() {

    //Serial.println(*name + " adding child : "+string(nb_children));

    //Increment the number of children;
    nb_children++;

    //Reallocate the children array
    void *new_ptr = realloc(children, nb_children * sizeof(EEPROMTree **));

    //If the realloc fails (0 is returned)
    if (!new_ptr) {

        //Restore the number of children;
        nb_children--;

        //Display an error message;
        //TODO ERROR MESSAGE;

        Serial.println("Fail");

        //Fail;
        return false;
    }

    //Save the new pointer;
    children = (EEPROMTree **) new_ptr;

    return true;

}


/*
 * getChild : this function is used to retrieve a child by name;
 */

EEPROMTree *EEPROMTree::getChild(char *child_name) {

    //For every child :
    for (uint8_t child_id = 0; child_id < nb_children; child_id++) {

        //Cache the child pointer;
        EEPROMTree *child = children[child_id];

        //If the name of the child_id and our name are equal :
        if (compareStrings(child_name, children[child_id]->name->data())) {

            //Return the cached pointer to the child;
            return child;
        }

    }

    //If no child corresponds, returns a null pointer;
    return nullptr;

}


/*
 * getChild : this function is used to retrieve a child by name;
 */

EEPROMTree *EEPROMTree::getChild(uint8_t child_index) {

    //If the required child doesn't exist, fail;
    if (nb_children <= child_index)
        return nullptr;

    //Return a pointer to the required child;
    return children[child_index];

}


/*
 * createChildIfAbsent : this function will search among children the child with the given name, and return it;
 *
 *  If no child is found, it is created and returned;
 */

EEPROMTree *EEPROMTree::createChildIfAbsent(char *child_name) {

    //Cache the child found;
    EEPROMTree *child = getChild(child_name);

    //If the child doesn't exist (nullptr), create the child;
    if (!child) {

        //Create the child
        addChild(child_name);

        //Cache its pointer;
        child = children[nb_children - 1];

    }

    return child;

}


/*
 * getData : this function returns the value pointed by data, if it is assigned;
 */

const float EEPROMTree::getData() {

    //If data is not nullptr :
    if (data) {

        //Return the value addressed by data;
        return *data;
    }

    //If data is nullptr, return 0;
    return 0;

}

/*
 * getDataPointer : this function returns the data pointer's value;
 */

float *EEPROMTree::getDataPointer() {

    //Return the value of data;
    return data;

}


/*
 * setData : this function modifies the value addressed by data, if it is assigned;
 */

void EEPROMTree::setData(float value) {

    //If data if not nullptr;
    if (data) {

        //Modify the value addressed by data;
        *data = value;
    }

    //If data is nullptr, do nothing;

}


/*
 * resetData : this function resets the float variable to its default value;
 */

void EEPROMTree::resetData() {

    //If the pointer is not null :
    if (data) {

        //Reset the data to its default value;
        *data = default_value;
    }

}


/*
 * resetTree : this function resets the whole tree's data;
 */

void EEPROMTree::resetTree() {

    //First, reset the variable;;
    resetData();

    //Then, for each child :
    for (uint8_t child_index = 0; child_index < nb_children; child_index++) {

        //Reset its and all its children's data;
        children[child_index]->resetTree();

    }

}


/*
 * setData : this function modifies the value addressed by data, if it is assigned;
 */

void EEPROMTree::setDataPointer(float *data_pointer) {

    //Modify the data pointer;
    data = data_pointer;

}


/*
 * comparestrings : this function compares values of the two provided strings, and returns true if they are identical;;
 */

bool EEPROMTree::compareStrings(const char *string0, const char *string1) {

    //compare all chars :
    while (true) {

        //Cache the two chars;
        char c0 = *(string0++);

        //If chars are not the same : return false, as strings are equal;
        if (c0 != *(string1++)) return false;

        //If chars are the same, and not zero, compare next chars;
        if (c0) continue;

        //If both chars are null, succeed, as strings are the same;
        return true;

    }

}


/*
 * printTree : this recursive function prints the whole tree;
 */

tstring EEPROMTree::printTree() {


    //Initialise a string containing the name;
    tstring s = *name + "\n";

    //Add the content of each child;
    for (uint8_t child = 0; child < nb_children; child++) {

        s += children[child]->printTree();
    }

    //Return the complete string;
    return s;
}
