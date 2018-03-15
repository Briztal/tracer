//
// Created by root on 3/15/18.
//

#ifndef TRACER_LANGUAGE_H
#define TRACER_LANGUAGE_H


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


public:

    //Data initialisation function;
    virtual void initialise_data() = 0;

    //Send the initialisation message
    virtual void init_message() = 0;

    //Parse the message and eventually execute the associated command;
    virtual bool parse(char *message) = 0;

    //Generate the tree used to parse the message;
    virtual void generateTree() = 0;


};


#endif //TRACER_LANGUAGE_H
