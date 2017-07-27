/*
  CommandInterface.cpp - Part of TRACER

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
#include "../../config.h"
#ifdef ENABLE_TREE_INTERFACE

#include "TreeInterface.h"
#include "../../Core/Core.h"
#include "TreeInterfaceCommands.h"

#define BEGIN_BYTE (uint8_t)255
#define BEGIN_CHAR (char)-1

void TI::begin() {
    serial_begin(BAUDRATE);
    initialise_aliases();
    *data_out_0 = BEGIN_CHAR;
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);



}

/*
 * The function to initialise the command aliases
 */

void TI::initialise_aliases() {

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

#include "tree_interface_config.h"

}


/*
 * echo : this is a public shortcut allowing to send_packet echo data all aint32_t the code.
 *
 * The command id is the following : 0 (system canal) - 6 (echo canal);
 */
void TI::echo(const String msg) {

    prepare_data_out("\0\6",2);
    add_string_out(msg.c_str());
    send_packet();
}


/*
 * send_position : this is a public shortcut allowing to send_packet steppers position all aint32_t the code
 *      (t must be a float *, of size [NB_STEPPERS])
 *
 * The command id is the following : 0 (system canal) - 5 (steppers canal) - 0 (position canal);
 */
void TI::send_position(float *t) {
    prepare_data_out("\0\5\1", 3);
    for (uint8_t i = 0; i<NB_STEPPERS; i++) {
        add_float_out(t[i]);
    }
    send_packet();
}

/*
 * Two shortcuts to prepare standard packets
 */

void TI::prepare_structure_packet() {
    prepare_data_out("\0\0", 2);
}

void TI::prepare_system_packet() {
    prepare_data_out("\0", 1);
}

void TI::prepare_EEPROM_packet() {
    prepare_data_out("\0\1", 2);
}

//--------------------------------------------------Packet Emission-----------------------------------------------------

/*
 * prepare_data_out : the function to initialise a the next outgoing packet.
 *
 * It sets the command id bytes, and initialises the data_out size.
 */
void TI::prepare_data_out(const char *command_id, uint8_t command_id_size) {
    data_out = data_out_0+2;
    data_out_size = command_id_size + (uint8_t) 1;

    //Write the command id
    for (;command_id_size--;) {
        *(data_out++) = (*(command_id++));
    }

}

/*
 * add_X_out : the function to add a X data (X : char, int, int32_t, float, const char *) to the current packet
 *
 * They duplicate eventual 255,  updates the data_out_size, and copy the provided data to the current packet.
 *
 */


void TI::add_char_out(char data) {

    uint8_t space_count = 0;
    if (data_out_size  >= PACKET_SIZE) return;

    //Checking
    if (data == BEGIN_CHAR) {
        *data_out++ = data, space_count++;
    }
    *data_out++ = data, space_count++;
    data_out_size ++;
}

void TI::add_int_out(int data) {

    char *t = (char *) &data;
    uint8_t space_count = 0;
    if (data_out_size + 2 >= PACKET_SIZE) return;

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

void TI::add_float_out(float data) {
    char *t = (char *) &data;
    uint8_t space_count = 0;
    if (data_out_size + 4 >= PACKET_SIZE) return;

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

void TI::add_int32_t_out(int32_t data) {
    char *t = (char *) &data;
    uint8_t space_count = 0;
    if (data_out_size + 4 >= PACKET_SIZE) return;

    
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


void TI::add_string_out(const char *data) {
    //WARNING : NO CHECK FOR STRINGS

    char c;
    uint8_t space_count = 0;
    char *size_ptr = data_out++;
    data_out_size++;
    uint8_t data_out_space = (uint8_t) (PACKET_SIZE) - data_out_size;
    for (; data_out_space; data_out_space--, space_count++) {
        if (!(c = *data++))
            break;
        *data_out++ = c;
    }
    *size_ptr = space_count;
    data_out_size += space_count;

}

/*
 * send_packet : sends the packet in memory.
 *
 *      Before calling this function :
 *          - initialise_packet must have been called with the correct command_id and size.
 *          - All required data must have been added with the add_X_data functions.
 */
void TI::send_packet() {

    uint8_t size = data_out_size;

    *size_ptr = (char)(size-1);
    //serial_echo_byte((char)(size));
    char *ptr = data_out_0;
        for (size+=1;size--;) {
        serial_echo_byte(*ptr++);
    }
    serial_echo_byte((char)0);

    digitalWrite(13, (uint8_t)!digitalRead(13));

}

void TI::enqueue(char *command, uint8_t size) {

    char b = *command;

    size--;

#define GO_LOWER(c,name)\
    case c :\
        b = *(++command);\
        size--;\
        switch (b) {


#define CREATE_LEAF(c, fname)\
    case c :\
        ++command;/*Removing last command byte*/\
        Core::add_external_task(TreeInterfaceCommands::fname, command, size);/*Enqueue the correct task*/\
        return;

#define GO_UPPER\
    default:\
        return;\
    }\
    break;

    switch (b) {
        case 0 : 
            command++;

            TreeInterfaceCommands::system_canal_function(command, size);
            return;

#include "tree_interface_config.h"

        default:
            return;

    }

}

//---------------------------------------------------Reception----------------------------------------------------------

void TI::read_serial() {

    char r;
    while (serial_available()) {
        r = (char) serial_read();
        if (first_detected) {
            if (r == BEGIN_CHAR) {//Second time begin byte is detected : not a begin symbol.
                first_detected = false;
                in_data_remaining--;
            } else {//Not a double beginning byte -> begining symbol
                flush();
                if (r<PACKET_SIZE) {
                    packet_began = true;
                    in_data_size = in_data_remaining = (uint8_t) r;
                }
                return;
            }
        } else if (r == BEGIN_CHAR) {


            first_detected = true;
            return;
        }

        if (packet_began) {
            if (in_data_remaining-- != 0) {
                *(data_in++) = r;
            } else {
                enqueue(data_in_0, in_data_size);
                flush();
                return;
            }
        }
    }
}

void TI::flush() {
    first_detected = false;
    packet_began = false;
    in_data_remaining = 0;
    data_in = data_in_0;
}


void TI::send_tree_structure() {

    /*
     * Syntax :
     * reset_tree (beginning) :         0;
     * confirm_tree (end) :             1;
     * go_upper :                       2;
     * go_lower(i) :                    3, i;
     * create_leaf(i, name) :           4, i, name
     * require_arg(i, name) :           5, i, name
     * allow_multiplicity(i name)       6, i, name
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

#include "tree_interface_config.h"


    prepare_structure_packet();
    add_char_out(1);
    send_packet();
}


#define GO_UPPER
#define GO_LOWER(i, name)
#define CREATE_LEAF(i, name)\
    uint8_t TI::name##_size;\
    char tmp##name[MAX_DEPTH+1];\
    char *const TI::name##_id = tmp##name;\


#include "tree_interface_config.h"


bool TI::packet_began = false;
bool TI::first_detected = false;
uint8_t TI::in_data_remaining = 0;
uint8_t TI::in_data_size = 0;


char t_data_in[PACKET_SIZE];
char *TI::data_in = t_data_in;
char *const TI::data_in_0 = t_data_in;

char dout[PACKET_SIZE+2];
char *TI::data_out = dout;
char *const TI::data_out_0 = dout;
uint8_t TI::data_out_size;
char *const TI::size_ptr = dout+1;


#endif