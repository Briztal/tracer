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


#include <DataStructures/ArgumentsContainer.h>
#include <Control/Controllers/_controller_data.h>

class GCodeArguments {

    //--------------------------------------Arguments Storage--------------------------------------

public:

    //Clear the argument storage;
    static void clear();

    //Return the number of available spaces in the storage;
    static inline uint8_t available_spaces() {return arguments_storage.available_spaces();}

    //Inserts and saves an argument, and returns its index in the container;
    static inline bool insert_argument(const char *args, uint8_t *index_p) {return arguments_storage.insert_argument(args, index_p);}

    //Returns a pointer to an argument's first case, and gives the size;
    static inline char* get_argument(uint8_t index) {return arguments_storage.get_argument(index);}

    //Removes an argument;
    static inline void remove_argument(uint8_t index) {arguments_storage.remove_argument(index);}


private:

    //The arguments container;
    static ArgumentsContainer arguments_storage;


    //--------------------------------------Arguments parsing--------------------------------------

public:

    /*
     * A Gcode Inteface command can accept an undefined number of arguments, in an argument sequence.
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

    //Parse the provided arguments, and save the data in the local buffer;
    static bool parse_arguments(char *args_current_position);

    //Get the pointer to the required argument;
    static char *get_argument(char id);

    //Get a previously parsed argument_t if it exists;
    static float get_argument_value(char id);

    //Verify that all arguments (defined by their arguments) have been provided (arguments is null terminated);
    static bool verify_all_identifiers_presence(const char *identifiers);

    //Verify that at least one argument_t (defined by their arguments) have been provided (arguments is null terminated);
    static bool verify_one_identifiers_presence(const char *identifiers);

    //verify that an argument_t identifier has be provided;
    static bool verify_identifier_presence(char id);


private:

    //Number of arguments in a sequence;
    static uint8_t nb_identifiers;

    //Identifiers in a parsed argument_t sequence;
    static argument_t *const arguments;


};


#endif //TRACER_GCODEARGUMENTS_H
