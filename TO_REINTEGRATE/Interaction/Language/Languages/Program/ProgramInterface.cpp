/*
  TreeInterface.cpp - Part of TRACER

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
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <config.h>

#ifdef ENABLE_PROGRAM_INTERFACE

#include "ProgramInterface.h"
#include "TaskScheduler/TaskScheduler.h"
#include <hardware_language_abstraction.h>
#include <Project/InterfaceCommands/ProgramInterfaceCommands.h>
#include <Project/InterfaceCommands/_interface_data.h>

#define BEGIN_BYTE (uint8_t)255
#define BEGIN_CHAR (char)-1

void PI::init() {

    program_interface_link_t::begin();

    initialise_aliases();

    *data_out_0 = BEGIN_CHAR;

}


/*
 * The function to initialise_hardware the command aliases
 */

void PI::initialise_aliases() {

    char t[MAX_DEPTH + 1];
    uint8_t indice = 0;

#define GO_UPPER indice--;

#define GO_LOWER(i, name)\
    t[indice++] = i;

#define CREATE_LEAF(i, name)\
    for (uint8_t c = 0; c<indice; c++){\
        name##_id[c] = t[c];\
    }\
    name##_id[indice] = i;\
    name##_id[indice+1] = 0;\
    name##_size = indice+(uint8_t)1;

#include <Config/program_interface_config.h>

}


/*
 * echo : this is a public shortcut allowing to send_packet echo data all aint32_t the code.
 *
 * The command id is the following : 0 (system canal) - 6 (echo canal);
 */

void PI::echo(const string_t msg) {

    prepare_data_out("\0\6", 2);
    add_string_out(msg.c_str());
    send_packet();
}


/*
 * send_position : this is a public shortcut allowing to send_packet steppers position all aint32_t the code
 *      (t must be a float *, of size [NB_AXIS])
 *
 * The command id is the following : 0 (system canal) - 5 (steppers canal) - 0 (position canal);
 */
void PI::send_position(float *t) {
    prepare_data_out("\0\5\1", 3);
    for (uint8_t i = 0; i < NB_AXIS; i++) {
        add_float_out(t[i]);
    }
    send_packet();
}

/*
 * Two shortcuts to prepare standard packets
 */

void PI::prepare_structure_packet() {
    prepare_data_out("\0\0", 2);
}

void PI::prepare_system_packet() {
    prepare_data_out("\0", 1);
}

void PI::prepare_EEPROM_packet() {
    prepare_data_out("\0\1", 2);
}

//--------------------------------------------------Packet Emission-----------------------------------------------------

/*
 * prepare_data_out : the function to reset a the next outgoing packet.
 *
 * It sets the command id bytes, and initialises the data_out size.
 */

void PI::prepare_data_out(const char *command_id, uint8_t command_id_size) {
    data_out = data_out_0 + 2;
    data_out_size = command_id_size + (uint8_t) 1;

    //Write the command id
    for (; command_id_size--;) {
        *(data_out++) = (*(command_id++));
    }

}

/*
 * add_X_out : the function to add a X data (X : char, int, int32_t, float, const char *) to the current packet
 *
 * They duplicate eventual 255,  updates the data_out_size, and copy the provided data to the current packet.
 *
 */


void PI::add_char_out(char data) {

    uint8_t space_count = 0;
    if (data_out_size >= MAX_COMMAND_SIZE
) return;

    //Checking
    if (data == BEGIN_CHAR) {
        *data_out++ = data, space_count++;
    }
    *data_out++ = data, space_count++;
    data_out_size++;
}

void PI::add_int_out(int data) {

    char *t = (char *) &data;
    uint8_t space_count = 0;
    if (data_out_size + 2 >= MAX_COMMAND_SIZE
) return;

    //Checking on 2 bytes
    if (*t == BEGIN_CHAR) {
        *data_out++ = *t, space_count++;
    }
    *data_out++ = *t++, space_count++;

    if (*t == BEGIN_CHAR) {
        *data_out++ = *t, space_count++;
    }
    *data_out++ = *t, space_count++;
    data_out_size += space_count;

}

void PI::add_float_out(float data) {
    char *t = (char *) &data;
    uint8_t space_count = 0;
    if (data_out_size + 4 >= MAX_COMMAND_SIZE
) return;

    //Checking on 4 bytes
    if (*t == BEGIN_CHAR) {
        *data_out++ = *t, space_count++;
    }
    *data_out++ = *t++, space_count++;

    if (*t == BEGIN_CHAR) {
        *data_out++ = *t, space_count++;
    }
    *data_out++ = *t++, space_count++;

    if (*t == BEGIN_CHAR) {
        *data_out++ = *t, space_count++;
    }
    *data_out++ = *t++, space_count++;

    if (*t == BEGIN_CHAR) {
        *data_out++ = *t, space_count++;
    }
    *data_out++ = *t, space_count++;

    data_out_size += space_count;

}

void PI::add_int32_t_out(int32_t data) {
    char *t = (char *) &data;
    uint8_t space_count = 0;
    if (data_out_size + 4 >= MAX_COMMAND_SIZE
) return;


    //Checking on 4 bytes
    if (*t == BEGIN_CHAR) {
        *data_out++ = *t, space_count++;
    }
    *data_out++ = *t++, space_count++;

    if (*t == BEGIN_CHAR) {
        *data_out++ = *t, space_count++;
    }
    *data_out++ = *t++, space_count++;

    if (*t == BEGIN_CHAR) {
        *data_out++ = *t, space_count++;
    }
    *data_out++ = *t++, space_count++;

    if (*t == BEGIN_CHAR) {
        *data_out++ = *t, space_count++;
    }
    *data_out++ = *t, space_count++;

    data_out_size += space_count;
}


void PI::add_string_out(const char *data) {
    //WARNING : NO CHECK FOR STRINGS

    char c;
    uint8_t space_count = 0;
    char *size_ptr = data_out++;
    data_out_size++;
    uint8_t data_out_space = (uint8_t) (MAX_COMMAND_SIZE
) - data_out_size;
    for (; data_out_space; data_out_space--, space_count++) {
        if (!(c = *data++))
            break;
        *data_out++ = c;
    }
    *size_ptr = space_count;
    data_out_size += space_count;

}


//---------------------------------Functions called by TerminalInterfaceCommands------------------------------


/*
 * get_arguments : returns a pointer to the beginning of the required argument_t
 *
 */

char *ProgramInterface::get_arguments(uint8_t task_index, uint8_t *size) {
    return arguments_storage.get_argument(task_index, size);
}


/*
 * validate_task : removes the argument_t related to the task
 *
 */

void ProgramInterface::remove_arguments(uint8_t task_index) {
    arguments_storage.remove_argument(task_index);
}



/*
 * send_packet : sends the packet in memory.
 *
 *      Before calling this function :
 *          - initialise_packet must have been called with the correct command_id and size.
 *          - All required data must have been added with the add_X_data functions.
 */
void PI::send_packet() {

    uint8_t size = data_out_size;

    *size_ptr = (char) (size - 1);
    //serial_echo_byte((char)(size));
    char *ptr = data_out_0;
    for (size += 1; size--;) {
        program_interface_link_t::send_byte(*ptr++);
    }
    program_interface_link_t::send_byte((char) 0);


}

void PI::process_t(char *command, uint8_t size) {

    char b = *command;

    size--;

#define GO_LOWER(c, name)\
    case c :\
        b = *(++command);\
        size--;\
        switch (b) {


#define CREATE_LEAF(c, fname)\
    case c :\
        ++command;/*Removing last command byte*/\
        add_task(ProgramInterfaceCommands::_##fname, command, size);/*Enqueue the correct task*/\
        return;

#define GO_UPPER\
    default:\
        return;\
    }\
    break;

    switch (b) {
        case 0 :
            command++;

            ProgramInterfaceCommands::system_canal_function(command, size);

            return;

#include <Config/program_interface_config.h>

        default:
            return;

    }

}


/*
 * add_task : this function sends a task to the scheduler :
 *
 *  It starts by creating the struct to contain dynamic_args data, then it saves it.
 *
 *  Finally, it sends the task to task to the scheduler.
 *
 */

void PI::schedule_task(task_state_t (*task)(void *), char *command, uint8_t size) {
    //If the function fails,
    //if (!current_node->function(data_in, r_command_size)) {

    if (arguments_storage.data_spaces()) {

        uint8_t index = arguments_storage.insert_argument(command, size);

        //Create a struct in the heap to contain argument_t-related data.
        program_interface_data_t *data = new program_interface_data_t();
        data->arguments_index = index;

        //Create a task in the stack to contain task data
        task_t t = task_t();
        t.type = 255;
        t.dynamic_args = (void *)data;
        t.task = task;

        //Schedule the task
        TaskScheduler::schedule_task(t);

    }
}

//---------------------------------------------------Reception----------------------------------------------------------


/*
 * read_data : this function reads data on the communication layer.
 *
 *  If data is available, it reads it one char at time.
 *
 *  It must verify the special case, where the char BEGIN_CHAR appears alone (not followed by any other BEGIN_CHAR).
 *
 *      This case marks the beginning of a command
 *
 */
void PI::accept_data() {

    char r;

    while (program_interface_link_t::available()) {

        r = program_interface_link_t::read_data();

        PI::echo("SUUS "+String((uint8_t)r));

        if (first_detected) {
            //If a BEGIN_CHAR has just been detected :

            if (r == BEGIN_CHAR) {
                //Second time initialise_hardware byte is detected : not a initialise_hardware symbol.

                //Clear the flag
                first_detected = false;

                //Decrease the number of remaining chars, for the BEGIN_CHAR duplication
                in_data_remaining--;

            } else {
                //Not a double beginning byte -> beginning symbol

                //Flush data, and prepare to receive_all the command?
                reset_input_data();

                //If the command size is lesser than the maximal value, validate the receiving state
                if (r < MAX_COMMAND_SIZE) {

                    //Set the parsing flag
                    parsing_began = true;

                    //save the command size
                    in_data_size = in_data_remaining = (uint8_t) r;
                }

                //Go to the next char
                continue;

            }


        } else if (r == BEGIN_CHAR) {
            //If the last char was not a BEGIN_CHAR (case behind)

            //Set the flag
            first_detected = true;

            //Go to the next char
            continue;
        }

        //If the parsing is enabled, add the char to the input data :
        if (parsing_began) {

            //If the last char hasn't been processed
            if (in_data_remaining-- != 0) {

                //Add data
                *(data_in++) = r;

            } else {

                //If the last char has been processed

                //process_t the input data
                process_t(data_in_0, in_data_size);

                //Reset the input data
                reset_input_data();

                //Stop if no tasks data_spaces are available in the scheduler.
                if (!TaskScheduler::data_spaces())
                    return;

                continue;

            }
        }
    }
}


/*
 * reset_input_data : resets the input_data.
 *
 */
void PI::reset_input_data() {
    first_detected = false;
    parsing_began = false;
    in_data_remaining = 0;
    data_in = data_in_0;
}


/*
 * send_tree_structure : This function send the command structure to the listener.
 *
 *  This interface connects with a program that must adapt its behaviour to the command structure.
 *
 *  We use a basic delimiter (commands listed below) to make him acknowledge the command structure.
 *
 */

task_state_t PI::send_tree_structure() {

    /*
     * Syntax :
     * reset_tree (beginning) :         0;
     * confirm_tree (end) :             1;
     * go_upper :                       2;
     * go_lower(k1_position_indice) :                    3, k1_position_indice;
     * create_leaf(k1_position_indice, name) :           4, k1_position_indice, name
     * require_arg(k1_position_indice, name) :           5, k1_position_indice, name
     * allow_multiplicity(k1_position_indice name)       6, k1_position_indice, name
     */

    prepare_structure_packet();
    add_char_out(0);
    send_packet();

#define GO_UPPER\
    prepare_structure_packet();\
    add_char_out(2);\
    send_packet();

#define GO_LOWER(i, name)\
    prepare_structure_packet();\
    add_char_out(3);\
    add_char_out(i);\
    add_string_out(#name);\
    send_packet();

#define CREATE_LEAF(i, name)\
    prepare_structure_packet();\
    add_char_out(4);\
    add_char_out(i);\
    add_string_out(#name);\
    send_packet();


#define REQUIRE_ARG(type, name)\
    prepare_structure_packet();\
    add_char_out(5);\
    add_char_out(type);\
    add_string_out(#name);\
    send_packet();

#define ALLOW_MULTIPLICITY(count, name)\
    prepare_structure_packet();\
    add_char_out(6);\
    add_char_out(count);\
    add_string_out(#name);\
    send_packet();

#include <Config/program_interface_config.h>


    prepare_structure_packet();
    add_char_out(1);
    send_packet();


    return complete;
}




#define GO_UPPER
#define GO_LOWER(i, name)
#define CREATE_LEAF(i, name)\
    uint8_t PI::name##_size;\
    char tmp##name[MAX_DEPTH+1];\
    char *const PI::name##_id = tmp##name;\


#include <Config/program_interface_config.h>


bool PI::parsing_began = false;
bool PI::first_detected = false;
uint8_t PI::in_data_remaining = 0;
uint8_t PI::in_data_size = 0;

ArgumentsContainer PI::arguments_storage = ArgumentsContainer(MAX_COMMAND_SIZE, NB_PENDING_COMMANDS);


char t_data_in[MAX_COMMAND_SIZE];
char *PI::data_in = t_data_in;
char *const PI::data_in_0 = t_data_in;

char dout[MAX_COMMAND_SIZE
 + 2];
char *PI::data_out = dout;
char *const PI::data_out_0 = dout;
uint8_t PI::data_out_size;
char *const PI::size_ptr = dout + 1;


#endif