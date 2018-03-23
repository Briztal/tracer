//
// Created by root on 3/17/18.
//

#ifndef TRACER_GCODELANGUAGE_H
#define TRACER_GCODELANGUAGE_H


#include <Kernel/Interaction/Language/Abstracts/CharTreeLanguage.h>
#include <Kernel/Interaction/Language/Abstracts/CharArgumentLanguage.h>

class GCodeLanguage : public CharTreeLanguage, public CharArgumentLanguage {

    //--------------------------------------- Initialisation ---------------------------------------

public:

    //Constructor;
    GCodeLanguage(uint8_t max_children_nb, uint8_t max_arguments_nb);


    //--------------------------------------- encoding ---------------------------------------

public:

    //Encode a string;
    void encode(tstring &s, uint8_t type) final;


};


#endif //TRACER_GCODELANGUAGE_H
