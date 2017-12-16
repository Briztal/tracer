/*
  EEPROMInterface.h - Part of TRACER

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

#ifndef TRACER_EEPROMINTERFACE_H
#define TRACER_EEPROMINTERFACE_H


#include <cstdint>
#include <Control/Controllers/Terminal/TerminalTree.h>
#include "EEPROMNode.h"

class EEPROMInterface {

public:

    static void print_stored_data();

    static EEPROMNode *build_tree();

    static bool read_data_by_string(char *id_string, float *f);

    static void write_data_by_string(char *id_string, float value);

#ifdef ENABLE_PROGRAM_INTERFACE

    //Send the structure of the EEPROM through the ProcessInterface
    static void send_structure();

#endif

private:

    static EEPROMNode *eeprom_tree;

    static bool search_tree_by_string(char *data_in, float **data);

    static void search_log(EEPROMNode *node);

};


#endif //TRACER_EEPROMINTERFACE_H

