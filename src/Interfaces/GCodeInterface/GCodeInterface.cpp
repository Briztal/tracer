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
 */

void GCodeInterface::read_data() {


    //Don't process any data if no space is available in the argument_t sequence container
    if (!arguments_storage.available_spaces())
        return;

    while (gcode_interface_link_t::available()) {

        //Read the data link
        char read_char = gcode_interface_link_t::read();

        //If the recieved char is a line feed or a working_extruder return
        if ((read_char == 10) || (read_char == 13)) {

            //If a non empty uncorrupted packet has effectively been received
            if (command_size && !corrupted_packet) {

                //Parse the GCode
                parse();

                //Reset the data_in
                reset();

                if (!TaskScheduler::available_spaces(255)) {
                    return;
                }

                //Don't process any data if no space is available in the argument_t sequence container
                if (!arguments_storage.available_spaces()) {
                    return;
                }

            }

            //If the received packet was too long for the input buffer :
            if (corrupted_packet) {

                //Display an error message
                GI::echo("WARNING in TerminalInterface::read_data : the received packet was too long for "
                                 "the input buffer. Please check your terminal_interface_config.h");


            }

            //If the packet was corrupted, or empty
            reset();


        } else {
            //If the packet hasn't been entirely received

            //If data still can be inserted in the buffer
            if (data_spaces) {

                //Append the read_output char to data_in;
                *(data_in++) = read_char;

                //Increment the command size;
                command_size++;

                //Decrement the number of spaces available;
                data_spaces--;

            } else {

                //Mark the current packet as corrupted;
                corrupted_packet = true;
            }

        }
    }
}


/*
 * reset : this function resets the parsing structure;
 */

void GCodeInterface::reset() {

    //No data;
    command_size = 0;

    //Clear the corruption flag;
    corrupted_packet = false;

    //Maximum numbers of char spaces;
    data_spaces = GCODE_MAX_SIZE;

    //data insertion at the origin;
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

    //Update data_in
    data_in += command_id_length;

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

    //Confirm the command execution
    confirm_command_execution(data);

    //Return the execution state.
    return state;
}


/*
 * confirm_command_execution : this command notifies the interlocutor of the execution state of the command.
 *
 *  As the terminal interface is a human-only interface, we will simply display a message to the user.
 */

void GCodeInterface::confirm_command_execution(const interface_data_t *data) {

//The log will occur only if the command flag is set.

#ifdef GCODE_EXECUTION_CONFIRMATION

    //Switch the return state.
    switch(data->return_state) {

        //If the task completed correctly
        case complete:
            CI::echo("Command complete.");
            return;

            //If the task completed correctly
        case invalid_arguments:
            CI::echo("Invalid Arguments.");
            return;

            //If the task must be reprogrammed:
        case reprogram:
            //CI::echo("Command Reprogrammed.");
            return;

            //If the task completed correctly
        default:
            CI::echo("WARNING Unrecognised return state");
            return;

    }

#endif


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

bool GCodeInterface::parse_arguments(char *args_current_position) {

    //First, reset the argument parsing structure :

    //reset the arguments counter
    nb_identifiers = 0;

    do {

        //First, remove unnecessary spaces;
        uint8_t dec = StringUtils::lstrip(args_current_position, ' ');

        args_current_position+= dec;
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
            CI::echo("The TerminalInterface hasn't been configured to accept more than " + String(GCODE_MAX_ARGS_NB) +
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

        //----------------------------------Argument saving----------------------------------


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
        argument_t *link = arguments + i;

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

ArgumentsContainer m::arguments_storage = ArgumentsContainer(GCODE_MAX_ARGS_NB * (GCODE_MAX_WORD_SIZE + 4) + 1,
                                                               GCODE_MAX_PENDING_COMMANDS);



//Identifiers in a parsed argument_t sequence
argument_t t_gcode_arguments[GCODE_MAX_ARGS_NB];
argument_t *const m::arguments = t_gcode_arguments;

//Number of arguments in a sequence
uint8_t m::nb_identifiers = 0;

//The command size
unsigned char m::command_size;

//The current number of available spaces in the data bugger
uint8_t m::data_spaces = GCODE_MAX_SIZE;

//A flag set if the current packet is corrupted (too long for the data buffer)
bool m::corrupted_packet = false;

//Data pointers
char tdatain_gcode[GCODE_MAX_SIZE];
char *m::data_in = tdatain_gcode;
char *const m::data_in_0 = tdatain_gcode;


#undef m

#endif







