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


#include <DataStructures/StringUtils.h>
#include <Project/Config/gcode_interface_config.h>
#include <hardware_language_abstraction.h>
#include "Communication/Controllers.h"
#include "GCodeArguments.h"


/*
 * clear : this function resets the argument parsing environment;
 */

void GCodeArguments::clear() {

    //Clear the argument container;
    arguments_storage.clear();

    //Argument parsing initialisation;
    nb_identifiers = 0;

}



//--------------------------------------Arguments Processing--------------------------------------

/*
 * parse_arguments : this function parses an argument sequence, whose format is like below :
 *
 *  -i0 arg0 -i1 arg1 ... -in argn
 *
 *  After the parsing, arguments can be get with the get_argument methods.
 *
 *  Remarks :
 *      - argi can be empty.
 *      - The function DOES NOT COPY ARGS. All it does is saving the indices (char),
 *          and their eventual args's positions (char *). The argument sequence must NEVER be changed,
 *          or args values will be corrupted.
 */

bool GCodeArguments::parse_arguments(char *args_current_position) {

    //First, reset the argument parsing structure :

    //reset the arguments counter
    nb_identifiers = 0;

    do {

        //First, remove unnecessary nb_spaces;
        uint8_t dec = StringUtils::lstrip(args_current_position, ' ');

        args_current_position += dec;
        //The current position is now on an argument identifier, or the argument sequence's end.

        //Get the argument identifier.
        char argument_identifier = *args_current_position;

        //If we reached the end of the argument string :
        if (!argument_identifier) {

            //Complete;
            return true;

        }

        //Now we are sure to be parsing a new argument.

        //If the argument container is already full;
        if (nb_identifiers == GCODE_MAX_ARGS_NB) {

            //Display an error message
            CI::echo("The Terminal hasn't been configured to accept more than " + String(GCODE_MAX_ARGS_NB) +
                     " arguments. Please check your terminal_interface_config.h file.");

            //Fail
            return false;

        }

        //We are now sure not to be at the string's end position.
        //The eventual value is just after the current position.

        //Increment the current position (legit, as the string is not finished) and cache the new value.
        char *arg = ++args_current_position;

        //Make a local flag, that will determine if the parsing keeps on after the current argument.
        bool finished;


        //If the string end here (no value provided, and string end);
        if (!*arg) {

            //The argument is already at '\0' position, no modification is required.

            //Plan the stop of the loop
            finished = true;

            //Enqueue an empty argument
            goto insert_arg;

        }

        //Go to the word's end (if we are already, we will simply not move the position.
        args_current_position += StringUtils::count_until_char(args_current_position, ' ');

        //If the word's end is '\0' (String terminated), plan the loop stop.
        finished = (!*args_current_position);

        //Nullify the word's end (reason why we cached the bool.
        *args_current_position = '\0';

        //Shift the current position
        args_current_position++;

        //---------------------------------- Argument saving ----------------------------------

        insert_arg:

        //Save the relation between the identifier and the argument location.
        argument_t *argument = arguments + nb_identifiers;
        argument->identifier = argument_identifier;
        argument->arg = arg;

        //Increase the number of parsed arguments
        nb_identifiers++;

        //If we have reached the end of the sequence
        if (finished) {

            //Complete
            return true;

        }


    } while (true);


}


/*
 * get_argument_value : this function presupposes that the argument referenced
 *  by the identifier id is a numeric value, and returns that value.
 */

float GCodeArguments::get_argument_value(char id) {

    //Get the argument pointer.
    char *ptr = get_argument(id);

    //return the float equivalent.
    if (ptr != nullptr) {

        return str_to_float(ptr);

    } else return 0;

}


/*
 * get_argument : this function returns a pointer to the argument (char *) referenced
 *  by the identifier id.
 */

char *GCodeArguments::get_argument(char id) {

    //For every identifier
    for (uint8_t i = 0; i < nb_identifiers; i++) {

        //Cache for the link
        argument_t *link = arguments + i;

        //If the links'identifier matches the provided one
        if (link->identifier == id) {

            //Get the location of the argument (from the link), and convert the argument into a float.
            return link->arg;

        }

    }

    return nullptr;

}


/*
 * verify_all_identifiers_presence : this function return true only if ALL arguments contained
 *  in the arguments string have been extracted during the previous parsing.
 *
 *  the identifier string is a string, where all letters are arguments to check .
 *      ex "arp" triggers the checking for arguments a, r and p.
 */

bool GCodeArguments::verify_all_identifiers_presence(const char *identifiers) {

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

    //All arguments are present, succeed.
    return true;

}


/*
 * verify_all_identifiers_presence : this function return true only if ONE arguments contained
 *  in the arguments string has been extracted during the previous parsing.
 *
 *  the identifier string is a string, where all letters are arguments to check .
 *      ex "arp" triggers the checking for arguments a, r and p.
 */

bool GCodeArguments::verify_one_identifiers_presence(const char *identifiers) {

    //Cache for the current char
    char c = *(identifiers++);

    //For every identifier in the string (stop at null byte);
    while (c) {

        //If the identifier is present
        if (verify_identifier_presence(c)) {

            //Succeed
            return true;

        }

        //Update the char
        c = *(identifiers++);

    }

    //No identifier detected, fail.
    return false;

}


/*
 * verify_all_identifiers_presence : this function return true if the identifier id has been extracted
 *  during the previous parsing.
 */

bool GCodeArguments::verify_identifier_presence(char id) {

    //Check every parsed identifier
    for (uint8_t i = 0; i < nb_identifiers; i++) {

        //Cache for the link
        argument_t *link = arguments + i;

        //If the links'identifier matches the provided one, return true
        if (link->identifier == id) return true;

    }

    //if there was no matches, return false.
    return false;

}



//Arguments container

ArgumentsContainer GCodeArguments::arguments_storage = ArgumentsContainer(GCODE_MAX_ARGS_NB * (GCODE_MAX_WORD_SIZE + 4) + 1,
                                                             GCODE_MAX_PENDING_COMMANDS);


//Identifiers in a parsed argument_t sequence
argument_t t_gcode_arguments[GCODE_MAX_ARGS_NB];
argument_t *const GCodeArguments::arguments = t_gcode_arguments;

//Number of arguments in a sequence
uint8_t GCodeArguments::nb_identifiers = 0;