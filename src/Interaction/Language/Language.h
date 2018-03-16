//
// Created by root on 3/15/18.
//

#ifndef TRACER_LANGUAGE_H
#define TRACER_LANGUAGE_H


#include <DataStructures/string/tstring.h>
#include <Kernel/TaskScheduler/task_state_t.h>

/*
 * This class is a virtual representation of a Language.
 *
 *  A Language is composed of two layers :
 *      - The syntax, the way to organise words;
 *      - The language itself, a tree of words (as they are defined by the syntax).
 *          Each node of the tree represents a command, that can be scheduled;
 *
 *  The language comprises the two last layers of the interaction stack;
 */

class Language {


    //-------------------------------------- Tree generation (for final class)  --------------------------------------

    //Generate the tree used to parse the message;
    virtual void generateTree() = 0;


    //-------------------------------------- Encoding / decoding --------------------------------------


    //Parse the message and eventually execute the associated command;
    virtual bool decode(const char *message) = 0;

    //Encode a rvalue string;
    virtual void encode(tstring &s, uint8_t type) = 0;



};


#endif //TRACER_LANGUAGE_H
