/*
  GCodeArguments.h - Part of TRACER

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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef TRACER_GCODEARGUMENTS_H
#define TRACER_GCODEARGUMENTS_H


#include <DataStructures/string/string.h>

#include <DataStructures/Containers/DynamicPointerBuffer.h>

#include <Interaction/Language/Language.h>

#include "stdint.h"



class CharArgumentLanguage : public virtual Language {


    //-------------------------------------- Initialisation --------------------------------------

public:

    //Default constructor;
    CharArgumentLanguage(uint8_t max_arguments_nb);

    //Default destructor;
    ~CharArgumentLanguage() = default;


    //--------------------------------------Arguments parsing--------------------------------------

public:

    /*
     * A char argument command can accept an undefined number of arguments, in an argument sequence.
     *
     *  An argument sequence is a sequence of word, where the first letter is the identifier, and
     *      the rest of the word is an eventual value, example
     *
     *      X100 Y200 Q300 R2.0214
     *
     *  where all i_k are characters, and arg_k is a word (meaning anything). The character - is mandatory
     *      before an identifier, as arg_k can be none (empty string).
     *
     */

    //Parse the provided content, and save the data in the local buffer;
    bool parseArguments(const char *args_current_position) final;

    //CLear the parsing content;
    void clear();

    //Get the pointer to the required argument;
    const char * get_argument(char id);

    //Get a previously parsed argument_t if it exists;
    float get_argument_value(char id);

    //Verify that all content (defined by their content) have been provided (content is null terminated);
    bool verify_all_identifiers_presence(const char *identifiers);

    //Verify that at least one argument_t (defined by their content) have been provided (content is null terminated);
    bool verify_one_identifiers_presence(const char *identifiers);

    //verify that an argument_t identifier has be provided;
    bool verify_identifier_presence(char id);


private:

    /*
     * To contain argument data, we will define a class that will contain a char identifier and an string;
     */

    class ArgumentData {

    public:

        //The argument identifier;
        char identifier;

        //The argument data, a string containing the argument value;
        string data;

    };

    //The parsed arguments buffer;
    DynamicPointerBuffer<ArgumentData> arguments;


};


#endif //TRACER_GCODEARGUMENTS_H