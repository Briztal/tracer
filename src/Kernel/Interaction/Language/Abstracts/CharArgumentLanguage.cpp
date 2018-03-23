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

#include <Config/control_config.h>

#ifdef ENABLE_GCODE_INTERFACE

#include "CharArgumentLanguage.h"


#include "Kernel/Interaction/Interaction.h"

//-------------------------------------- Initialisation --------------------------------------

/*
 * Constructor : initialises the arguments array;
 */
CharArgumentLanguage::CharArgumentLanguage(uint8_t max_arguments_nb) : arguments(max_arguments_nb) {

}


//--------------------------------------Arguments Processing--------------------------------------

/*
 * parseArguments : this function parses an argument sequence, whose format is like below :
 *
 *  -i0 arg0 -i1 arg1 ... -in argn
 *
 *  After the parsing, content can be get with the get_argument methods.
 */

bool CharArgumentLanguage::parseArguments(const char *argument_sequence) {

#define RETURN_IF_NULL(char_array)\
    /*If we reached the end of the argument string :*/\
    if (!*(char_array)) {\
        \
        /*Complete;*/\
        return true;\
        \
    }

    //First, clear the argument parsing structure :
    arguments.clear();

    //Parse all arguments;
    do {

        //First, remove unnecessary nb_spaces;
        argument_sequence += cstring::lstrip(argument_sequence, ' ');

        //Then, go to the next argument identifier;
        argument_sequence += cstring::count_until_char(argument_sequence, '-');

        //The current position is now on an argument separator, or the argument sequence's end.

        RETURN_IF_NULL(argument_sequence);

        //Increment the position to obtain the argument identifier;
        argument_sequence++;

        RETURN_IF_NULL(argument_sequence);

        //Get the argument identifier (not null).
        char argument_identifier = *(argument_sequence);

        //Increment the car pointer;
        argument_sequence++;

        //Now, we must get the value;

        //Remove unnecessary nb_spaces;
        argument_sequence += cstring::lstrip(argument_sequence, ' ');

        //Save the pointer to the arg. Can point to '\0';
        const char *arg = argument_sequence;

        //Determine the arguments size (0 if points to '\0');
        uint16_t argument_size = cstring::count_until_char(argument_sequence, '-');

        //Go to the word's end;
        argument_sequence += argument_size;

        //---------------------------------- Argument saving ----------------------------------

        //Save the relation between the identifier and the argument location.
        ArgumentData *argumentData = new ArgumentData();
        argumentData->identifier = argument_identifier;
        argumentData->data = string(arg, argument_size);

        //Add the argument to the buffer;
        //If the argument container is already full;
        if (!arguments.add(argumentData)) {

            //Display an error message
            std_out("Error in CharArgumentLanguage::parseArguments : too many arguments. Check your config file;");

            //Fail
            return false;

        }

    } while (*argument_sequence);


}


/*
 * clear : this function resets the argument parsing environment;
 */

void CharArgumentLanguage::clear() {

    //Clear the argument container;
    arguments.clear();

}


/*
 * get_argument_value : this function presupposes that the argument referenced
 *  by the identifier id is a numeric value, and returns that value.
 */

float CharArgumentLanguage::get_argument_value(char id) {

    //Get the argument pointer.
    const char *ptr = get_argument(id);

    //return the float equivalent.
    if (ptr != nullptr) {

        std_out("TODO  String to float");
        //TODO STRING TO FLOAT
        return 0;// str_to_float(ptr);

    } else return 0;

}


/*
 * get_argument : this function returns a pointer to the argument (char *) referenced
 *  by the identifier id.
 */

const char *CharArgumentLanguage::get_argument(char id) {

    //For every identifier
    for (uint8_t argument_index = arguments.getSize(); argument_index--;) {

        //Cache for the link
        ArgumentData *link = arguments.getElement(argument_index);

        //If the links'identifier matches the provided one
        if (link->identifier == id) {

            //Get the location of the argument (from the link), and convert the argument into a float.
            return link->data.data();

        }

    }

    //If no valid identifier was found : fail;
    return nullptr;

}


/*
 * verify_all_identifiers_presence : this function return true only if ALL content contained
 *  in the content string have been extracted during the previous parsing.
 *
 *  the identifier string is a string, where all letters are content to check .
 *      ex "arp" triggers the checking for content a, r and p.
 */

bool CharArgumentLanguage::verify_all_identifiers_presence(const char *identifiers) {

    //Cache for the current char
    char c = *(identifiers++);

    //For every identifier in the string (stop at null byte);
    while (c) {

        //If the identifier is not present
        if (!verify_identifier_presence(c)) {

            //Fail
            return false;

        }

        //Update the char
        c = *(identifiers++);

    }

    //All content are present, succeed.
    return true;

}


/*
 * verify_all_identifiers_presence : this function return true only if ONE content contained
 *  in the content string has been extracted during the previous parsing.
 *
 *  the identifier string is a string, where all letters are content to check .
 *      ex "arp" triggers the checking for content a, r and p.
 */

bool CharArgumentLanguage::verify_one_identifiers_presence(const char *identifiers) {

    //Cache for the current char
    char c;

    //For every identifier in the string (stop at null byte);
    while ((c = *(identifiers++))) {

        //If the identifier is present
        if (verify_identifier_presence(c)) {

            //Succeed
            return true;

        }

    }

    //No identifier detected, fail.
    return false;

}


/*
 * verify_all_identifiers_presence : this function return true if the identifier id has been extracted
 *  during the previous parsing.
 */

bool CharArgumentLanguage::verify_identifier_presence(char id) {

    //Check every parsed identifier
    for (uint8_t argument_index = arguments.getSize(); argument_index--;) {

        //If the links'identifier matches the provided one, return true
        if (arguments.getElement(argument_index)->identifier == id) return true;

    }

    //if there was no matches, return false.
    return false;

}


#endif