//
// Created by root on 23/10/17.
//

#include "EEPROMNode.h"


EEPROMNode::EEPROMNode(string_t *name, uint8_t sub_nodes_nb, uint8_t index, float *const data) :

        name(name), sub_nodes_nb(sub_nodes_nb),index(index), data(data)

{
    sub_nodes = (EEPROMNode **) malloc(sub_nodes_nb * sizeof(EEPROMNode *));

}