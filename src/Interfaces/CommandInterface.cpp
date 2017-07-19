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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "CommandInterface.h"
#include "../Core/MachineControllerSystem.h"
#include "../config.h"
#include "../Core/Core.h"

#define BEGIN_BYTE (unsigned char)255
#define BEGIN_CHAR (char)-1

void CI::begin() {
    serial_begin(BAUDRATE);
    initialise_aliases();
    delay_ms(100);
    *data_out_0 = BEGIN_CHAR;

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);



}

/*
 * The function to initialise the command aliases
 */

void CI::initialise_aliases() {

    char t[MAX_DEPTH + 1];
    unsigned char indice = 0;

#define GO_UPPER indice--;

#define GO_LOWER(i, name)\
    t[indice++] = i;

#define CREATE_LEAF(i, name)\
    for (unsigned char c = 0; c<indice; c++){\
        name##_id[c] = t[c];\
    }\
    name##_id[indice] = i;\
    name##_id[indice+1] = 0;\
    name##_size = indice+(unsigned char)1;

#include "interface_config.h"

}


/*
 * echo : this is a public shortcut allowing to send_packet echo data all along the code.
 *
 * The command id is the following : 0 (system canal) - 6 (echo canal);
 */
void CI::echo(const String msg) {

    prepare_data_out("\0\6",2);
    add_string_out(msg.c_str());
    send_packet();
}


/*
 * send_position : this is a public shortcut allowing to send_packet steppers position all along the code
 *      (t must be a float *, of size [NB_STEPPERS])
 *
 * The command id is the following : 0 (system canal) - 5 (steppers canal) - 0 (position canal);
 */
void CI::send_position(float *t) {
    prepare_data_out("\0\5\1", 3);
    for (unsigned char i = 0; i<NB_STEPPERS; i++) {
        add_float_out(t[i]);
    }
    send_packet();
}

/*
 * Two shortcuts to prepare standard packets
 */

void CI::prepare_structure_packet() {
    prepare_data_out("\0\0", 2);
}

void CI::prepare_system_packet() {
    prepare_data_out("\0", 1);
}

void CI::prepare_EEPROM_packet() {
    prepare_data_out("\0\1", 2);
}

//--------------------------------------------------Packet Emission-----------------------------------------------------

/*
 * prepare_data_out : the function to initialise a the next outgoing packet.
 *
 * It sets the command id bytes, and initialises the data_out size.
 */
void CI::prepare_data_out(const char *command_id, unsigned char command_id_size) {
    data_out = data_out_0+2;
    data_out_size = command_id_size + (unsigned char) 1;

    //Write the command id
    for (;command_id_size--;) {
        *(data_out++) = (*(command_id++));
    }

}

/*
 * add_X_out : the function to add a X data (X : char, int, long, float, const char *) to the current packet
 *
 * They duplicate eventual 255,  updates the data_out_size, and copy the provided data to the current packet.
 *
 */


void CI::add_char_out(char data) {

    unsigned char space_count = 0;
    if (data_out_size  >= PACKET_SIZE) return;

    //Checking
    if (data == BEGIN_CHAR) {
        *data_out++ = data, space_count++;
    }
    *data_out++ = data, space_count++;
    data_out_size ++;
}

void CI::add_int_out(int data) {

    char *t = (char *) &data;
    unsigned char space_count = 0;
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

void CI::add_float_out(float data) {
    char *t = (char *) &data;
    unsigned char space_count = 0;
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

void CI::add_long_out(long data) {
    char *t = (char *) &data;
    unsigned char space_count = 0;
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


void CI::add_string_out(const char *data) {
    //WARNING : NO CHECK FOR STRINGS

    char c;
    unsigned char space_count = 0;
    char *size_ptr = data_out++;
    data_out_size++;
    unsigned char data_out_space = (unsigned char) (PACKET_SIZE) - data_out_size;
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
void CI::send_packet() {

    unsigned char size = data_out_size;

    *size_ptr = (char)(size-1);
    //serial_echo_byte((char)(size));
    char *ptr = data_out_0;
        for (size+=1;size--;) {
        serial_echo_byte(*ptr++);
    }
    serial_echo_byte((char)0);

    digitalWrite(13, (uint8_t)!digitalRead(13));

}

void CI::enqueue(char *command, unsigned char size) {



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
        Core::add_external_task(MachineController::fname, command, size);/*Enqueue the correct task*/\
        return;

#define GO_UPPER\
    default:\
        return;\
    }\
    break;

    switch (b) {
        case 0 : 
            command++;
            MachineController::system_canal_function(command, size);
            return;

#include "interface_config.h"

        default:
            return;

    }

}

//---------------------------------------------------Reception----------------------------------------------------------

void CI::read_serial() {

    char r;
    while (serial_available()) {
        Serial.print("\1");
        r = (char) serial_read();
        Serial.print(r);
        if (first_detected) {
            if (r == BEGIN_CHAR) {//Second time begin byte is detected : not a begin symbol.
                first_detected = false;
                in_data_remaining--;
            } else {//Not a double beginning byte -> begining symbol
                flush();
                if (r<PACKET_SIZE) {
                    packet_began = true;
                    in_data_size = in_data_remaining = (unsigned char) r;
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

void CI::flush() {
    first_detected = false;
    packet_began = false;
    in_data_remaining = 0;
    data_in = data_in_0;
}


void CI::send_tree_structure() {

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

#include "interface_config.h"


    prepare_structure_packet();
    add_char_out(1);
    send_packet();
}


#define GO_UPPER
#define GO_LOWER(i, name)
#define CREATE_LEAF(i, name)\
    unsigned char CI::name##_size;\
    char tmp##name[MAX_DEPTH+1];\
    char *const CI::name##_id = tmp##name;\


#include "interface_config.h"


bool CI::packet_began = false;
bool CI::first_detected = false;
unsigned char CI::in_data_remaining = 0;
unsigned char CI::in_data_size = 0;


char t_data_in[PACKET_SIZE];
char *CI::data_in = t_data_in;
char *const CI::data_in_0 = t_data_in;

char dout[PACKET_SIZE+2];
char *CI::data_out = dout;
char *const CI::data_out_0 = dout;
unsigned char CI::data_out_size;
char *const CI::size_ptr = dout+1;
