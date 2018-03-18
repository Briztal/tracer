//
// Created by root on 3/18/18.
//

#ifndef TRACER_STRINGTREELANGUAGE_H
#define TRACER_STRINGTREELANGUAGE_H

#include <Interaction/Language/Language.h>

#include "DataStructures/Containers/Tree.h"


typedef Tree<string, language_function> StringTree;


class StringTreeLanguage : virtual public Language {


    //-------------------------------------- Initialisation ----------------------------------

public :

    //Default constructor;
    StringTreeLanguage(uint8_t max_string_length, uint8_t max_children_nb);

    //Destructor;
    ~StringTreeLanguage() = default;


    //--------------------------------------Decoding ---------------------------------

public:

    //Parse the ByteTreeLanguage Command.
    bool decode(const char *message) final;



    //------------------------------------ Tree Creation -----------------------------

public:

    //Add a null terminated command to the tree;
    void addCommand(const char *command_name, language_function);


protected:

    //The ByteTreeLanguage Command tree;
    StringTree commandTree;

    //The maximum number of children in our tree;
    uint8_t max_children_nb;

};


#endif //TRACER_STRINGTREELANGUAGE_H
