/*
  TerminalArguments.cpp - Part of TRACER

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

#ifdef ENABLE_TERMINAL_INTERFACE

#include "TerminalArguments.h"

#include "Config/controller_terminal_config.h"
#include <Interaction/Interaction.h>
#include <DataStructures/StringUtils.h>


/*
 * clear : this function resets the argument parsing environment;
 */

void TerminalArguments::clear() {

    //Clear the argument container;
    arguments_storage.clear();

    //Argument parsing initialisation;
    nb_identifiers = 0;

}

//--------------------------------------Arguments Processing--------------------------------------

/*
 * parse_arguments : this function parses an argument_t sequence, whose format is like below :
 *
 *  -i0 arg0 -i1 arg1 ... -in argn
 *
 *  After the parsing, content can be get with the get_argument methods.
 *
 *  Remarks :
 *      - argi can be empty.
 *      - The function DOES NOT COPY ARGS. All it does is saving the indices (char),
 *          and their eventual args's positions (char *). The argument sequence must NEVER be changed,
 *          or args values will be corrupted.
 */

bool TerminalArguments::parse_arguments(char *argument_sequence) {

    //A cache with a more relevant name
    char *current_position = argument_sequence;

    //First, reset the argument_t parsing structure :

    //reset the content counter
    nb_identifiers = 0;


    do {

        //go to the closest argument_t identifier
        current_position += StringUtils::count_until_char(current_position, '-');

        //If we have reached the end of the sequence
        if (!*current_position)
            return true;

        //Get the pointer to the identifier;
        char identifier = *(current_position + 1);

        //Get the argument_t identifier
        uint8_t identifier_size = StringUtils::count_until_char(current_position, ' ');

        //Go to the next unprocessed char
        current_position += identifier_size;


        //If the identifier is invalid
        if (identifier_size != 2) {

            //Display an error message
            std_out("INVALID ARGUMENT IDENTIFIER");

            //Fail
            return false;

        }

        //If the argument_t container is already full;
        if (nb_identifiers == TERMINAL_MAX_ARGS_NB) {

            //Display an error message
            std_out(
                    "The TerminalArguments hasn't been configured to accept more than " + String(TERMINAL_MAX_ARGS_NB) +
                    " content. Please check your terminal_interface_config.h file.");

            //Fail
            return false;

        }


        //remove extra nb_spaces;
        current_position += StringUtils::lstrip(current_position, ' ');

        char *arg;
        char *space_position;
        uint8_t argument_size;

        //If the argument sequence is finished;
        if (!*current_position) {

            //The arg will point to the zero ending the sequence
            arg = current_position;

            //An empty argument must be added;
            goto insert_arg;

        }

        //The current position corresponds now to a '-', or to the beginning of an argument.
        //We now must parse the argument, if it is provided.

        //Count the numbers of chars between the current position and an argument identifier (or the end of the string);
        argument_size = StringUtils::count_until_char(current_position, '-');

        //If no argument is provided :
        if (!argument_size) {

            //The arg will point to the space (changed in zero below) before the current position;
            arg = current_position - 1;

            //Change the space to a zero;
            *arg = '\0';

            //An empty argument must be added;
            goto insert_arg;

        }

        //The argument now exists for sure, but we must ensure that there is a '\0' at its end.

        //If we ended the argument sequence, no need to add the zero.
        if (!*(current_position + argument_size)) {

            //The arg will point to the zero ending the sequence
            arg = current_position;

            goto insert_arg;

        }

        //We are now sure that a '-' is present.

        //Normally, a space should be present just before the '-';
        space_position = current_position + argument_size - 1;

        //If the char at the space position is effectively a char:
        if (*space_position == ' ') {

            //Nullify it;
            *space_position = '\0';

            //Arg will point to the current position (the beginning of the argument.
            arg = current_position;

            //Update the current position the new '-' (or at the end of the sequence).
            current_position += argument_size;

            //Insert the non empty arg;
            goto insert_arg;

        } else {

            //If the '-' was in a word, display an error message;
            std_out("Invalid content, please do not use '-' in a word.");

            //Fail;
            return false;

        }

        //----------------------------------Argument saving----------------------------------


        insert_arg:

        //Save the relation between the identifier and the argument_t location.
        argument_t *argument = arguments + nb_identifiers;
        argument->identifier = identifier;
        argument->arg = arg;


        //Increase the number of parsed content
        nb_identifiers++;

        //If we have reached the end of the sequence
        if (!*current_position)
            return true;


    } while (true);


}

/*
 * get_argument_value : this function presupposes that the argument_t referenced
 *  by the identifier id is a numeric value, and returns that value.
 */

float TerminalArguments::get_argument_value(char id) {

    //Get the argument_t pointer.
    char *ptr = get_argument(id);

    //return the float equivalent.
    if (ptr != nullptr) {
        return str_to_float(ptr);
    } else return 0;

}


/*
 * get_argument : this function returns a pointer to the argument_t (char *) referenced
 *  by the identifier id.
 */

char *TerminalArguments::get_argument(char id) {

    //For every identifier
    for (uint8_t i = 0; i < nb_identifiers; i++) {

        //Cache for the link
        argument_t *link = arguments + i;

        //If the links'identifier matches the provided one
        if (link->identifier == id) {

            //Get the location of the argument_t (from the link), and convert the argument_t into a float.
            return link->arg;

        }

    }

    return nullptr;

}


/*
 * verify_all_identifiers_presence : this function return true only if ALL content contained
 *  in the content string have been extracted during the previous parsing.
 *
 *  the identifier string is a string, where all letters are content to check .
 *      ex "arp" triggers the checking for content a, r and p.
 */

bool TerminalArguments::verify_all_identifiers_presence(const char *identifiers) {

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

bool TerminalArguments::verify_one_identifiers_presence(const char *identifiers) {

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
 * verify_identifier_presence : this function return true if the identifier id has been extracted
 *  during the previous parsing.
 */

bool TerminalArguments::verify_identifier_presence(char id) {

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


//Identifiers in a parsed argument_t sequence
argument_t terminal_arguments_t[TERMINAL_MAX_ARGS_NB];
argument_t *const TerminalArguments::arguments = terminal_arguments_t;

//Number of content in a sequence
uint8_t TerminalArguments::nb_identifiers = 0;

ArgumentsContainer TerminalArguments::arguments_storage = ArgumentsContainer(TERMINAL_MAX_ARGS_NB * (TERMINAL_MAX_WORD_SIZE + 4) + 1,
                                                                              TERMINAL_MAX_PENDING_COMMANDS);

#endif