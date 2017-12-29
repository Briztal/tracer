/*
  EEPROM.h - Part of TRACER

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

#ifndef TRACER_EEPROM_H
#define TRACER_EEPROM_H

#include "EEPROMTree.h"

class EEPROMMap {

    //--------------------------------------------- Initialisation -----------------------------------------------------

public:

    //Initialise the tree;
    static void initialise_data();

    //--------------------------------------------- Tree population -----------------------------------------------------

public:

    //Add a branch in the tree, by its label;
    static void add_branch(char *id_string, float *data_pointer);

    //Add a branch in the tree, by its label;
    static void add_branch(char *id_string, EEPROMTree *data_pointer);

    //Disable any branch add, until the class is reset;
    static void lock_tree();


private:

    //The lock flag. If it is set, the data tree is const;
    static bool lock_flag;

    //The data tree;
    static EEPROMTree *data_tree;


private:

    //Create a path if it is not present;
    static EEPROMTree *createIfAbsent(char *id_string);



    //------------------------------------------- EEPROM Interaction ---------------------------------------------------

public:

    //Save the data tree in the EEPROM;
    static void save_eeprom_data();

    //Read the data tree from the EEPROM;
    static bool load_eeprom_data();


private:

    //Hash the data tree in the hash array;
    static void hash_tree();

    //Hash a tree (recursive)
    static void _hash_tree(EEPROMTree *tree, uint8_t *index_p);

    //Verify that the hash in the hash array matches the one in the EEPROM;
    static bool compare_hash();

    //Write the hash in the EEPROM;
    static bool write_hash();

    //Load a tree's data from the EEPROM;
    static bool _load_eeprom_data(EEPROMTree *tree, uint8_t *eeprom_index_p);

    //Save a tree's data in the EEPROM;
    static void _save_eeprom_data(EEPROMTree *tree, uint8_t *eeprom_index_p);


private:

    //The hash array;
    static uint8_t *const hash_array;



    //-------------------------------------------- Search functions ----------------------------------------------------

public:

    //Read data
    static bool read_data_by_string(char *id_string, float *f);

    //Write data
    static void write_data_by_string(char *id_string, float value);

    //Reset all the data tree;
    static void reset();

    //Reset a particular child;
    static void reset(char *id_string);

    //Print the content of a particular child;
    static void print_tree(char *id_string);


private:

    //Log a bad research;
    static void search_log(EEPROMTree *tree);

    //Search a tree by its path;
    static EEPROMTree *search_tree_by_string(char *data_in);

};


#endif //TRACER_EEPROM_H
