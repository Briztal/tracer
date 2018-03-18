//
// Created by root on 3/17/18.
//

#include "GCodeLanguage.h"



GCodeLanguage::GCodeLanguage(uint8_t max_children_nb, uint8_t max_arguments_nb) :
        CharTreeLanguage(max_children_nb), CharArgumentLanguage(max_arguments_nb) {}


/*
 * encode : encodes the message string;
 */

void GCodeLanguage::encode(tstring &message, uint8_t type) {

}