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


#include <DataStructures/Containers/Tree.h>
#include "DataStructures/string/string.h"

/*
 * To define the type EEPROMTree, we will need a node class. It will contain a float pointer and a default value;
 */
class EEPROMData {

public:

    EEPROMData() : data_pointer(nullptr), default_value(0) {}

    EEPROMData(float *data_pointer, float default_value) : data_pointer(data_pointer), default_value(default_value) {}


public:

    //The variable we keep track of;
    float *data_pointer = nullptr;

    //The variable's default value;
    float default_value = 0;



};

//Now, we can define the Storage Tree type;
typedef Tree<string, EEPROMData> EEPROMTree;


namespace EEPROMMap {

    //--------------------------------------------- Initialisation -----------------------------------------------------
    
    //Initialise the tree;
    void initialise_data();
    

    //--------------------------------------------- Tree population -----------------------------------------------------

    //Add a branch in the tree, by its label;
    void add_branch(const char *id_string, float *data_pointer);

    //Add a branch in the tree, by its label;
    void add_branch(const char *id_string, EEPROMTree *data_pointer);

    //Disable any branch add, until the class is reset;
    void lock_tree();
    

    //------------------------------------------- Storage Interaction ---------------------------------------------------
    
    //Save the data tree in the Storage;
    void save_eeprom_data();

    //Read the data tree from the Storage;
    bool load_eeprom_data();


    //-------------------------------------------- Search functions ----------------------------------------------------
    
    //Read data
    bool read_data_by_string(const char *id_string, float *f);

    //Write data
    void write_data_by_string(const char *id_string, float value);

    //Reset a particular child;
    void reset(const char *id_string);

    //Print the content of a particular child;
    void print_tree(const char *id_string);
    

};


#endif //TRACER_EEPROM_H
