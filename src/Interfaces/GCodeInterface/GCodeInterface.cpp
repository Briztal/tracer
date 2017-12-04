/*
  GCodeInterface.cpp - Part of TRACER

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
#include <config.h>

#ifdef ENABLE_GCODE_INTERFACE

#include "GCodeInterface.h"
#include <Project/InterfaceCommands/GCodeCommands.h>
#include <DataStructures/StringUtils.h>


//----------------------------------------------------Initialisation----------------------------------------------------

/*
 * init : this function initialises the link layer.
 *
 *  It is automatically called by the Scheduler at initialisation
 *
 */

void GCodeInterface::init() {

    gcode_interface_link_t::begin();

    delay_ms(100);

}


//-----------------------------------------------------Data reading-----------------------------------------------------

/*
 * read_data : this function reads and processes data.
 *
 *  It reads data on the link layer, saves it, and eventually processes it.
 *
 */

void GCodeInterface::read_data() {

    while (gcode_interface_link_t::available()) {

        //Read the data link
        char read_char = gcode_interface_link_t::read();

        //If the recieved char is a line feed or a working_extruder return
        if ((read_char == 10) || (read_char == 13)) {

            //If a char has effectively been received
            if (command_size) {

                //Parse the GCode
                parse();

                //Reset the data_in
                reset();

                if (!TaskScheduler::available_spaces(255));
                return;

            }

        } else {

            //Append the read_output char to data_in
            *(data_in++) = read_char;

            command_size++;

        }
    }
}


/*
 * reset : this function resets the parsing structure
 *
 */

void GCodeInterface::reset() {

    //No data
    command_size = 0;

    //data insertion at the origin
    data_in = data_in_0;
}


//---------------------------------------------------------Parse--------------------------------------------------------


/*
 * parse : this function parses the received GCode Command.
 *
 *  It is called when a GCode command has been entirely received
 */

bool GCodeInterface::parse() {


    //Initialise the parsing.
    init_parsing();

    //Declare the command id length (ex : 4 for G130).
    unsigned char command_id_length = 0;

    //Declare an array to receive the command id (must add one byte for null termination)
    char command_id[GCODE_MAX_DEPTH + 1];

    //Get the command id and its length, by calling the StringUtils.
    command_id_length = StringUtils::get_next_word(data_in, command_id, GCODE_MAX_DEPTH + 1);

    //Abort if the first word was a space.
    if (!command_id_length)
        return false;

    //Analyse the command id.
    analyse_command(command_id, command_id_length);

    return true;

}


/*
 * init_parsing : this function initialises the data parsing.
 *
 *  It positions the data pointer to the beginning of the command, and resets all parameters flags.
 */

void GCodeInterface::init_parsing() {

    //Mark the end of the the received command
    *data_in = 0;

    //Reset the data pointer
    data_in = data_in_0;

}


/*
 * analyse_command : this function determines the function associated to a GCode Command.
 *
 *  If it succeeds, it calls the scheduling function, passing a pointer to the determines function.
 */

void GCodeInterface::analyse_command(char *command, unsigned char command_size) {

    //Initialise a char containing the letter we focus on
    char c = *(command++);

    //As the first letter is saved, decrease the size.
    command_size--;

    echo(String(command_size));\

    //TODO COMMENT THE PARSING


#define COMMAND(i, fname) \
    case i : \
        schedule(GCodeCommands::fname);\
        return;

#define GO_LOWER(i) \
    case i : \
        if (!(command_size--)) return;\
        c = *(command++);\
        switch(c) {\

#define GO_LOWER_COMMAND(i, fname) \
    case i : \
        echo(String(command_size));\
        if (!(command_size--)) {schedule(GCodeCommands::fname);return;}\
        c = *(command++);\
        switch(c) {\


#define GO_UPPER()\
        default : return;\
    }\

    switch (c) {

#include <Project/Config/gcode_interface_config.h>

        default:
            return;
    }

#undef COMMAND
#undef GO_LOWER
#undef GO_LOWER_COMMAND
#undef GO_UPPER

}


/*
 * schedule : this function schedules a GCodeCommands command.
 *
 *  It saves the arguments part in the argument storage, and then, schedules the execution of the function passed
 *      in arguments
 *
 *  This command will be executed, by the intermediary of the execute_command function, defined below.
 *      (see the doc above the execute_command function's definition for more explanations).
 */

void GCodeInterface::schedule(task_state_t (*f)(char *)) {

    //Save the current arguments
    uint8_t index;

    //Copy the argument string in the argument storage, and get the index it was inserted at.
    arguments_storage.insert_argument(data_in, &index);

    //Create a struct in the heap to contain argument-related data.
    interface_data_t *data = new interface_data_t();
    data->arguments_index = index;
    data->function= f;

    //Schedule the task
    TaskScheduler::schedule_task(255, execute_command, (void*) data);

}


/*
 * execute_command : this command eases the redaction of TerminalCommands :
 *
 *  Normally, if scheduled directly, a GCodeCommand receives a void *, that must be casted in a char *,
 *      the address of the arguments sequence. After the execution, if the command mustn't be reprogrammed,
 *      those arguments must be removed from the argument container.
 *
 *      As this procedure is common to all GCodeCommands, this function's goal is to execute this procedure.
 *
 *  Now, a GCodeCommand receives a simple char *, and this function does the following :
 *      - extracting arguments (char *);
 *      - executing the GCodeCommand, passing these args.
 *      - eventually removing the arguments.
 */

task_state_t GCodeInterface::execute_command(void *data_pointer) {

    //Get the terminal interface data back
    interface_data_t *data = (interface_data_t *) data_pointer;

    //Cache var for arguments index.
    uint8_t arguments_index = data->arguments_index;

    //Cache for arguments.
    char *arguments = arguments_storage.get_argument(arguments_index);

    //Execute the required TerminalCommand function, and get the execution state
    const task_state_t state = (*data->function)(arguments);

    /*remove arguments arguments, if the task mustn't be reprogrammed*/
    if (state != reprogram) {
        arguments_storage.remove_argument(arguments_index);
    }

    //Return the execution state.
    return state;
}


//------------------------------------------------------Parameters------------------------------------------------------


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

bool GCodeInterface::parse_arguments(char *args_current_position) {

    //First, reset the argument parsing structure :

    //reset the arguments counter
    nb_identifiers = 0;


    do {


        //First, remove unnecessary spaces;
        args_current_position += StringUtils::lstrip(args_current_position, ' ');

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
        if (nb_identifiers == MAX_ARGS_NB) {

            //Display an error message
            CI::echo("The TerminalInterface hasn't been configured to accept more than " + String(MAX_ARGS_NB) +
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

        //----------------------------------Argument saving----------------------------------


        insert_arg:

        //Save the relation between the identifier and the argument location.
        argument_t *argument = identifiers + nb_identifiers;
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

float GCodeInterface::get_argument_value(char id) {

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

char *GCodeInterface::get_argument(char id) {

    //For every identifier
    for (uint8_t i = 0; i < nb_identifiers; i++) {

        //Cache for the link
        argument_t *link = identifiers + i;

        //If the links'identifier matches the provided one
        if (link->identifier == id) {

            //Get the location of the argument (from the link), and convert the argument into a float.
            return link->arg;

        }

    }

    return nullptr;

}


/*
 * verify_all_identifiers_presence : this function return true only if ALL identifiers contained
 *  in the identifiers string have been extracted during the previous parsing.
 *
 *  the identifier string is a string, where all letters are identifiers to check .
 *      ex "arp" triggers the checking for identifiers a, r and p.
 */

bool GCodeInterface::verify_all_identifiers_presence(const char *identifiers) {

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

    //All identifiers are present, succeed.
    return true;

}

/*
 * verify_all_identifiers_presence : this function return true only if ONE identifiers contained
 *  in the identifiers string has been extracted during the previous parsing.
 *
 *  the identifier string is a string, where all letters are identifiers to check .
 *      ex "arp" triggers the checking for identifiers a, r and p.
 */

bool GCodeInterface::verify_one_identifiers_presence(const char *identifiers) {

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

bool GCodeInterface::verify_identifier_presence(char id) {

    //Check every parsed identifier
    for (uint8_t i = 0; i < nb_identifiers; i++) {

        //Cache for the link
        argument_t *link = identifiers + i;

        //If the links'identifier matches the provided one, return true
        if (link->identifier == id) return true;

    }

    //if there was no matches, return false.
    return false;

}

//----------------------------------------------------System aliases----------------------------------------------------


/*
 * echo : this function is an alias for the system echo command.
 *
 *  It echoes text data on the link layer
 */

void GCodeInterface::echo(const string_t msg) {

    gcode_interface_link_t::send_str(msg + "\n");

}

//-------------------------------------------Static declarations / definitions------------------------------------------

#define m GCodeInterface




//Arguments container

ArgumentsContainer m::arguments_storage = ArgumentsContainer(MAX_ARGS_NB * (MAX_WORD_SIZE + 4) + 1,
                                                               NB_PENDING_COMMANDS);



//Identifiers in a parsed argument_t sequence
argument_t t_id_to_indexes[MAX_ARGS_NB];
argument_t *const m::identifiers = t_id_to_indexes;

//Number of arguments in a sequence
uint8_t m::nb_identifiers = 0;

//The command size
unsigned char m::command_size;

//Data pointers
char tdatain_gcode[GCODE_MAX_SIZE];
char *m::data_in = tdatain_gcode;
char *const m::data_in_0 = tdatain_gcode;


#undef m

#endif







