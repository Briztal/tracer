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
#include <Project/InterfaceCommands/_interface_data.h>
#include <Project/InterfaceCommands/GCodeInterfaceCommands.h>
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
 *
 */

bool GCodeInterface::parse() {


    //Initialise the parsing.
    init_parsing();

    //Declare the command id length (ex : 4 for G130).
    unsigned char command_id_length = 0;

    //Get the command id and its length, by calling the StringUtils.
    command_id_length = StringUtils::get_next_word(&data_in, &command_size);

    //Abort if the first word was a space.
    if (!command_id_length)
        return false;

    //declare a char array to contain the command id (ex : G130).
    char command_id[GCODE_MAX_DEPTH]{0};

    //Copy the command ID into our local array.
    memcpy(command_id, StringUtils::word_buffer_0, sizeof(char) * command_id_length);

    //Analyse the command id.
    analyse_command(command_id, command_id_length);

    return true;

}


/*
 * init_parsing : this function initialises the data parsing.
 *
 *  It positions the data pointer to the beginning of the command, and resets all parameters flags.
 *
 */

void GCodeInterface::init_parsing() {

    //Reset the data pointer
    data_in = data_in_0;

    //Reset parameter flags
    memset(dynamic_args.parameters_flags, sizeof(float) * NB_PARAMETERS, 0);

}


/*
 * analyse_command : this function determines the function associated to a GCode Command.
 *
 *  If it succeeds, it calls the scheduling function, passing a pointer to the determines function.
 *
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
        schedule(GCodeInterfaceCommands::fname);\
        return;

#define GO_LOWER(i) \
    case i : \
        if (!(command_size--)) return;\
        c = *(command++);\
        switch(c) {\

#define GO_LOWER_COMMAND(i, fname) \
    case i : \
        echo(String(command_size));\
        if (!(command_size--)) {schedule(GCodeInterfaceCommands::fname);return;}\
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
 * schedule : this function schedules a GCodeInterfaceCommands command.
 *
 *  It simply creates a task, saves the current parameters, packs the data, and gives it to the Scheduler.
 *
 */

void GCodeInterface::schedule(task_state_t (*f)(void *)) {

#ifdef PARSE_BEFORE_EXECUTION

    //If we must parse parameters before the execution, parse them. If it fails, abort.
    if (!parse_parameters(data_in, command_size))
        return;

    //Save the current parameters
    uint8_t index = arguments_storage.insert_argument((char *) &dynamic_args, sizeof(gcode_arguments));

#else

#endif

    //Create a struct in the heap to contain argument-related data.
    gcode_interface_data_t *data = new gcode_interface_data_t();
    data->arguments_index = index;

    //Create a task in the stack to contain task data
    task_t t = task_t();
    t.type = 255;
    t.dynamic_args = (void *) data;
    t.task = f;

    //Schedule the task
    TaskScheduler::add_task(t);

}


//------------------------------------------------------Parameters------------------------------------------------------


/*
 * parse_parameters : this function will parse the parameter string provided in arguments
 */
bool GCodeInterface::parse_parameters(char *parameter_buffer, uint8_t params_size) {

    //Initialise the parsing environment.
    parameters_size = params_size;
    parameters_ptr = parameter_buffer;


    //Some variables for the current parameter's index and value.
    char index;
    float value;

    //While the parameters_size is not null
    while (parameters_size) {

        /*
         * Get the next parameter index and value
         * If the parsing fails (incorrect parameter format), abort.
         */
        if (!get_parameter(&index, &value)) {
            return false;
        }

        /*
         * Process the parsed data.
         * If it fails (wrong parameter index) abort.
         */
        if (!process_parameter(index, value)) {
            return false;
        }

    }

    return true;
}


/*
 * get_parameter : this function reads parameters_ptr and extract the next word, supposed to be a parameter (index + value)
 *
 *  It calls the StringUtils to get the word, and then reads the index, and parses the value.
 *
 */
bool GCodeInterface::get_parameter(char *id, float *value) {


    //Get the parameter id, its value and its length, by calling the StringUtils
    uint8_t size = StringUtils::get_next_word(&parameters_ptr, &parameters_size);

    //Abort if no argument was parsed
    if (size == 0)
        return false;

    //Save the parameter id
    *id = *StringUtils::word_buffer_0;

    //Do not parse the rest of the string if only the parameter index was provided
    if (size == 1) {

        //Default value
        *value = 0;

        //Complete
        return true;
    }

    //Parse the value
    *value = str_to_float(StringUtils::word_buffer_0 + 1);

    //Complete
    return true;

}


/*
 * process_parameter : this function processes a parameter (index and value).
 *
 *  It searches for a parameter matching the given index, and if it is not already set, sets it to the provided value.
 *
 */

bool GCodeInterface::process_parameter(char index, float value) {

    //Run through every parameter
    switch (index) {

        //For each parameter :
#define GPARAMETER(i, j, k)\
            /*if the parameter matched the given index*/\
            case j : \
                /*if the parameter is not already set*/\
                if (!*(dynamic_args.parameters_flags + i)){\
                    /*set the flag, and save the value*/\
                    *(dynamic_args.parameters + i) = value;\
                    *(dynamic_args.parameters_flags + i) = true;\
                }\
                return true;

#include <Project/Config/gcode_interface_config.h>

#undef GPARAMETER


        default:
            //If the given index doesn't match any parameter, fail.
            return false;

    }

}


//----------------------------------------------------System aliases----------------------------------------------------


/*
 * echo : this function is an alias for the system echo command.
 *
 *  It echoes text data on the link layer
 *
 */

void GCodeInterface::echo(const string_t msg) {

    gcode_interface_link_t::send_str(msg + "\n");

}

//-------------------------------------------Static declarations / definitions------------------------------------------

#define m GCodeInterface


//Arguments container
ArgumentsContainer m::arguments_storage = ArgumentsContainer(sizeof(gcode_arguments), NB_PENDING_TASKS);

//The command size
unsigned char m::command_size;
unsigned char m::parameters_size;

//Data pointers
char tdatain_gcode[GCODE_MAX_SIZE];
char *m::data_in = tdatain_gcode;
char *const m::data_in_0 = tdatain_gcode;

//Parameters pointers
char *m::parameters_ptr;

//Current arguments
gcode_arguments m::dynamic_args = gcode_arguments();

#undef m

#endif







