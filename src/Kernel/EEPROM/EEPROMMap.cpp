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

#include <Config/eeprom_config.h>
#include "EEPROMMap.h"


namespace EEPROMMap {

    //--------------------------------------------- Tree population -----------------------------------------------------

    //Create a path if it is not present;
    EEPROMTree *create_if_absent(const char *id_string);


    //The lock flag. If it is set, the data tree is const;
    bool lock_flag = false;

    //The data tree;
    EEPROMTree *data_tree = new EEPROMTree(string("root"), EEPROMData(), 0);;


    //------------------------------------------- EEPROM Interaction ---------------------------------------------------

    //Hash the data tree in the hash array;
    void hash_tree();

    //Hash a tree (recursive)
    void _hash_tree(EEPROMTree *tree, uint8_t *index_p);

    //Verify that the hash in the hash array matches the one in the EEPROM;
    bool compare_hash();

    //Write the hash in the EEPROM;
    bool write_hash();

    //Load a tree's data from the EEPROM;
    bool _load_eeprom_data(EEPROMTree *tree, uint8_t *eeprom_index_p);

    //Save a tree's data in the EEPROM;
    void _save_eeprom_data(EEPROMTree *tree, uint8_t *eeprom_index_p);


    //The hash array;
    uint8_t hash_array[EEPROM_HASH_SIZE];;


    //-------------------------------------------- Tree  functions ----------------------------------------------------

    //Reset a particular tree's data;
    void reset_tree(EEPROMTree *);


    //-------------------------------------------- Search functions ----------------------------------------------------

    //Log a bad research;
    void search_log(EEPROMTree *tree);

    //Search a tree by its path;
    EEPROMTree *search_tree_by_string(const char *data_in, const bool authorise_non_leafs);

}

//--------------------------------------------- Initialisation -----------------------------------------------------


void EEPROMMap::initialise_data() {

    //Delete the data tree;
    delete data_tree;

    //Instantiate a new data tree;
    data_tree = new EEPROMTree(string("root"), EEPROMData(), 255);

    //Enable the tree modification;
    lock_flag = false;

}


//--------------------------------------------- Tree population -----------------------------------------------------


/*
 * add_branch : this function adds a new branch in the data tree, containing the provided data pointer;
 *
 *  The name string provided contains the name of the new tree that will be created;
 */

void EEPROMMap::add_branch(const char *id_string, float *data_pointer) {

    //If the tree is locked, fail;
    if (lock_flag) {
        return;
    }

    //Create the required path if necessary;
    EEPROMTree *tree = create_if_absent(id_string);

    //Update the data pointer;
    EEPROMData *data = tree->getNode();

    //Modify the data pointer;
    data->data_pointer = data_pointer;

}


/*
 * add_branch : this function appends the provided tree to the specified tree's children
 *
 *  The name string provided contains the name of the tree that will contain the child;
 */

void EEPROMMap::add_branch(const char *id_string, EEPROMTree *child) {

    //If the tree is locked, fail;
    if (lock_flag) {
        return;
    }

    //Create the required path if necessary;
    EEPROMTree *tree = create_if_absent(id_string);

    //Update the data pointer;
    tree->addChild(child);

}


/*
 * create_if_absent : this function will create the required path in the data tree if it doesn't exist.
 *
 *  It returns the tree named ny the path;
 */

EEPROMTree *EEPROMMap::create_if_absent(const char *id_string) {

    //Initialise a pointer to the tree we currently examine children;
    EEPROMTree *current_tree = data_tree;

    //While the identifier string is not finished :
    while (*id_string) {

        //Remove extra spaces at the beginning of the string;
        id_string += cstring::lstrip(id_string, ' ');

        //Cache the beginning of the word;
        const char *word_ptr = id_string;

        //Get the size of the next word;
        uint8_t word_size = cstring::count_until_char(id_string, ' ');

        //If the word is empty, stop;
        if (!word_size)
            return current_tree;

        //Copy the word with the size;
        string label(word_ptr, word_size);

        //Update the id string;
        id_string += word_size;

        //Get the child corresponding to the word;
        EEPROMTree *child_tree = current_tree->getChild(label);

        //If the child doesn't exist :
        if (child_tree == nullptr) {

            //Create the child;
            child_tree = new EEPROMTree(label, EEPROMData(), MAX_EEPROM_CHILDREN_NB);

            //Add the child to the current tree;
            current_tree->addChild(child_tree);

        }

        current_tree = child_tree;

    }

    //return the tree corresponding to the specified path;
    return current_tree;

}


/*
 * lock_tree : this function disables any modification in the data tree;
 */

void EEPROMMap::lock_tree() {

    //Disable the tree modification;
    lock_flag = true;

    //Hash the tree;
    hash_tree();

    //Try to oad the tree data;
    bool b = load_eeprom_data();

    //If the loading failed, save the current tree;
    if (!b) {

        Serial.println("FAILED TO LOAD A RELEVANT PROFILE IN THE EEPROM. ");

        save_eeprom_data();
    }

}



//------------------------------------------- EEPROM Interaction ---------------------------------------------------


/*
 * hash_tree : this function initialises the hashing procedure and calls he recursive function on data_tree;
 */

void EEPROMMap::hash_tree() {

    //Initialise the hash index at the end of the array;
    uint8_t index = EEPROM_HASH_SIZE - 1;

    //Reset the hash array;
    memset(hash_array, 0, EEPROM_HASH_SIZE * sizeof(uint8_t));

    //Hash the tree;
    _hash_tree(data_tree, &index);

}


/*
 * _hash_tree : this function hashes a tree and calls itself on its children;
 */

void EEPROMMap::_hash_tree(EEPROMTree *tree, uint8_t *index_p) {

    //This macro will decrement i if it is not null, or set it to EEPROM_HASH_SIZE if it is null;
#define SAFE_DECR(i) {if (!(i)) {(i) = EEPROM_HASH_SIZE;}(i)--;}

    //First, we will add the name.

    //Cache the index. This variable is static, not used during self-call;
    static uint8_t index;
    index = *index_p;

    //A cache for the name;
    static const char *name;
    name = tree->getLabel().data();

    //Cache variable for the current char;
    static char current_char;

    //For each char in the name :
    while ((current_char = (*name++))) {

        //Add the int value of the char;
        hash_array[index] += (uint8_t) current_char;

        //Decrement safely the index;
        SAFE_DECR(index);

    }

    //If the data pointer is not null, we will add 1 and decr;
    if (tree->getNode()->data_pointer) {

        //Add 1;
        hash_array[index] += (uint8_t) 1;

        //Decrement safely the index;
        SAFE_DECR(index);

    }

    //Now, we will add the number of children.

    //Add the number of children;
    hash_array[index] += tree->getChildrenNb();

    //Decrement safely the index;
    SAFE_DECR(index);


    //Update index_p;
    *index_p = index;


    //Then, we will add children to the hash.

    //Cache the number of children;
    const uint8_t nb_children = tree->getChildrenNb();

    //For every child :
    for (uint8_t child_index = nb_children; child_index--;) {

        //Add the child data to the hash;
        _hash_tree(tree->getChild(child_index), index_p);

    }

#undef SAFE_DECR

}


/*
 * compare_hash : this function compares the hash present in the EEPROM, and the current hash;
 */

bool EEPROMMap::compare_hash() {

    //For each byte of the hash
    for (uint8_t i = 0; i < EEPROM_HASH_SIZE; i++) {

        //If the byte is different than the one in the EEPROM :
        if (hash_array[i] != (uint8_t) EEPROM_read(i)) {

            //Fail
            return false;

        }

    }

    //If all bytes are the same, complete;
    return true;

}


/*
 * write_hash : this function compares the hash present in the EEPROM, and the current hash;
 */

bool EEPROMMap::write_hash() {

    //For each byte of the hash
    for (uint8_t i = 0; i < EEPROM_HASH_SIZE; i++) {

        //Write the hash;
        EEPROM_write(i, hash_array[i]);

    }

    //If all bytes are the same, complete;
    return true;

}


/*
 * read_tree : readall the data tree from the EEPROM;
 */

bool EEPROMMap::load_eeprom_data() {

    //If the tree is not locked :
    if (!lock_flag) {

        //Error message
        std_out("Error in EEPROMMap::load_eeprom_data : "
                        "A tree load has been attempted when the data tree was not locked. Aborted.");

        //Fail;
        return false;
    }

    //If the hash in the EEPROM doesn't match the current one :
    if (!compare_hash()) {

        //Fail;
        return false;

    }

    //Initialise the EEPROM Index;
    uint8_t eeprom_index = EEPROM_HASH_SIZE;

    //Save the data tree in the EEPROM;
    return _load_eeprom_data(data_tree, &eeprom_index);

}


/*
 * _load_eeprom_data : this function loads data at the given index for the provided tree, and calls itself on
 *      its children;
 */

bool EEPROMMap::_load_eeprom_data(EEPROMTree *tree, uint8_t *eeprom_index_p) {

    //First, cache the value of eeprom_index;
    static uint8_t eeprom_index;

    eeprom_index = *eeprom_index_p;

    //Local data receiver;
    static float value = 0;

    //get the address of the receiver, and cast it as a byte pointer;
    static uint8_t *data_ptr;
    data_ptr = (uint8_t *) &value;

    //If the data pointer is not null :
    if (tree->getNode()->data_pointer != nullptr) {

        //For each byte of data :
        for (uint8_t byte_index = 0; byte_index < 4; byte_index++) {

            //Update the byte value and increment the pointer;
            *(data_ptr++) = EEPROM_read(eeprom_index);

            //Increment the eeprom_index;
            eeprom_index++;

        }

        //Verify that the value is a number;
        if (isnanf(value))
            return false;

        //Update the tree's value;
        *(tree->getNode()->data_pointer) = value;

        //Update the index pointer;
        *eeprom_index_p = eeprom_index;

    }

    //For each child :
    for (uint8_t child_index = tree->getChildrenNb(); child_index--;) {

        //load the child's data;
        bool b = _load_eeprom_data(tree->getChild(child_index), eeprom_index_p);

        //If an error is detected, fail.
        if (!b)
            return false;

    }

    //If no errors were detected, complete;
    return true;

}


/*
 * write_tree : save the data tree in the EEPROM;
 */

void EEPROMMap::save_eeprom_data() {

    //If the tree is not locked :
    if (!lock_flag) {

        //Error message
        std_out("Error in EEPROMMap::save_eeprom_data : "
                        "A tree save has been attempted when the data tree was not locked. Aborted.");

        //Fail;
        return;
    }

    //If the hash in the EEPROM doesn't match the current one :
    if (!compare_hash()) {

        //Write the hash in the EEPROM;
        write_hash();

    }

    //Initialise the eeprom index to the end of the hash;
    uint8_t eeprom_index = EEPROM_HASH_SIZE;

    //Save all the data in the EEPROM;
    _save_eeprom_data(data_tree, &eeprom_index);

}


/*
 * _load_eeprom_data : this function loads data at the given index for the provided tree, and calls itself on
 *      its children;
 */

void EEPROMMap::_save_eeprom_data(EEPROMTree *tree, uint8_t *eeprom_index_p) {

    //First, cache the value of eeprom_index;
    static uint8_t eeprom_index;
    eeprom_index = *eeprom_index_p;

    //get the address of the tree's data, and cast it as a byte pointer;
    static uint8_t *data_ptr;
    data_ptr = (uint8_t *) tree->getNode()->data_pointer;

    //If the data pointer is not null :
    if (data_ptr) {

        //For each byte of data :
        for (uint8_t byte_index = sizeof(float) / sizeof(uint8_t); byte_index--;) {

            //Cache the byte value (good practice for a macro call);
            uint8_t byte = *(data_ptr++);

            //Update the byte value and increment the pointer;
            EEPROM_write(eeprom_index, byte);

            //Increment the eeprom_index;
            eeprom_index++;

        }

        //Update the index pointer;
        *eeprom_index_p = eeprom_index;

    }

    //For each child :
    for (uint8_t child_index = tree->getChildrenNb(); child_index--;) {

        //save the child's data;
        _save_eeprom_data(tree->getChild(child_index), eeprom_index_p);

    }

}


//-------------------------------------------- Search functions ----------------------------------------------------


/*
 * read_data_by_string : this function will return the value of the variable indexed by the tree
 *  corresponding to id_string.
 */

bool EEPROMMap::read_data_by_string(const char *id_string, float *f) {

    //Get the required tree among leafs;
    EEPROMTree *tree = search_tree_by_string(id_string, false);

    //If the tree is not null
    if (tree) {
        *f = *tree->getNode()->data_pointer;
        return true;
    } else {
        return false;
    }

}


/*
 * write_data_by_string : this function will update the value of the variable indexed by the tree
 *  corresponding to id_string.
 */

void EEPROMMap::write_data_by_string(const char *id_string, float value) {

    //Get the required tree among leafs;
    EEPROMTree *tree = search_tree_by_string(id_string, false);


    //If the tree is not null :
    if (tree) {

        //Update its data;
        *(tree->getNode()->data_pointer) = value;

    }

}


/*
 * reset : this function resets a particular variable;
 */

void EEPROMMap::reset(const char *id_string) {

    //Get the required tree. Non-leafs are authorised;
    EEPROMTree *tree = search_tree_by_string(id_string, true);

    //If the tree is not null :
    if (tree) {

        //reset its variable;
        tree->recursiveCall(reset_tree);

    }

}


/*
 * reset_tree : this function resets recursively a tree;
 */

void EEPROMMap::reset_tree(EEPROMTree *tree) {

    //Cache the EEPROM Data pointer;
    EEPROMData *data = tree->getNode();

    //Reset the variable;
    *data->data_pointer = data->default_value;

}


/*
 * print_tree this function will print the entire content of the data tree;
 */

void EEPROMMap::print_tree(const char *id_string) {

    //Get the required tree. Non-leafs are authorised;
    EEPROMTree *tree = search_tree_by_string(id_string, true);

    //If the tree is not null :
    if (tree) {

        //TODO RECURSIVE PRINT
        //Display the content of the tree;
        //std_out(tree->printTree());

    }

}


/*
 * search_tree_by_string : this function searches and returns (if it exists) the tree corresponding to the
 *  identifier string provided.
 *
 *  If it doesn't exist, it returns 0;
 */

EEPROMTree *EEPROMMap::search_tree_by_string(const char *data_in, const bool authorise_non_leafs) {

    //Initialise the current current_tree to the root;
    EEPROMTree *current_tree = data_tree;

    while (true) {

        //remove extra spaces
        data_in += cstring::lstrip(data_in, ' ');

        //Go to the next word;
        uint8_t word_size = cstring::count_until_char(data_in, ' ');

        //Copy the words;
        const string s(data_in, word_size);

        //If the string is finished :
        if (!word_size) {

            break;
        }

        //Update the current char;
        data_in += word_size;

        //Increment the next char if the string is not finished;
        if (*data_in) {
            data_in++;
        }

        //Get the child matching the given string;
        EEPROMTree *child_tree = current_tree->getChild(s);

        //If the child doesn't exist :
        if (child_tree == nullptr) {

            //Search log;
            search_log(current_tree);

            //Return null;
            return nullptr;
        }

        //If the tree is not null :


        //Re-initialise_hardware the current data
        current_tree = child_tree;

        //if the new node is a leaf:
        if (!current_tree->getChildrenNb()) {

            //Return the current child_tree;
            return current_tree;

        }

    }

    //Default case : string finished, tree is non leaf or root;

    //If non-leafs are authorised, return the tree.
    if (authorise_non_leafs) {

        current_tree;

    } else {

        //If not,log and return nullptr;
        search_log(current_tree);

        return nullptr;
    }

}


/*
 * search_log : this function will display help info when a research fails;
 */

void EEPROMMap::search_log(EEPROMTree *tree) {

    //Log message
    tstring s = tstring("Suggestions for ") + tree->getLabel() + " : ";

    //Cache the number of children;
    const uint8_t nb_children = tree->getChildrenNb();


    //Append the suggestion for every child tree;
    for (uint8_t i = 0; i < nb_children; i++) {
        s += tree->getChild(i)->getLabel() + ", ";

    }

    std_out(s);

}

