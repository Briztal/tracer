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

#ifndef TRACER_EEPROMNODE_H
#define TRACER_EEPROMNODE_H

#include <hardware_language_abstraction.h>

#include <DataStructures/Tree.h>

class EEPROMTree {


public:

    //The Constructor;
    EEPROMTree(string_t *name, float *data);

    //The Destructor
    ~EEPROMTree();


    //------------------------------------------- Getters -------------------------------------------

    //Get the number of children;
    const uint8_t getNbChildren();

    //Get the name;
    const String *getName();

    //Get the pointed value;
    const float getData();

    //Get a child by index;
    EEPROMTree *getChild(uint8_t child_index);

    //Get a child by name;
    EEPROMTree *getChild(char *child_name);


    //------------------------------------------- Modifiers -------------------------------------------

    //Modify the pointed value;
    void setData(float value);

    //Update the data pointer;
    void setDataPointer(float *data_pointer);

    //Add a child by name;
    void addChild(char *name);

    //Add a child by name;
    void addChild(EEPROMTree *child);

    //Get a child by index;
    EEPROMTree *createChildIfAbsent(char *child_name);


    //------------------------------------------- Info -------------------------------------------

    //Display the tree;
    String printTree();


    //------------------------------------------- Fields -------------------------------------------

private:

    //The current number of children;
    uint8_t nb_children;

    //The children array;
    EEPROMTree **children;

    //The parameter name;
    const String *name;

    //The pointer to the related data;
    float *data;


    //------------------------------------------- Private methods -------------------------------------------


    //Compare strings
    bool compareStrings(const char *string0, const char *string1);

    //Realloc children, and create a new space;
    bool _addChild();


};

#endif //TRACER_EEPROMNODE_H
