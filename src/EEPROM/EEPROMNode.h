//
// Created by root on 23/10/17.
//

#ifndef TRACER_EEPROMNODE_H
#define TRACER_EEPROMNODE_H

#include <hardware_language_abstraction.h>

class EEPROMNode {


public:

    EEPROMNode(string_t *name, uint8_t sub_nodes_nb, uint8_t id, float *const data);

    const String *name;

    const uint8_t sub_nodes_nb;

    const uint8_t index;

    float *const data;

    EEPROMNode ** sub_nodes;


};

#endif //TRACER_EEPROMNODE_H
