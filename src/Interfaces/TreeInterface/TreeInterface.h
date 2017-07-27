/*
  CommandInterface.h - Part of TRACER

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

#include "../../hardware_language_abstraction.h"
#if !defined(HL_SERIAL)
#error TreeInterface requires Serial. If your board and language supports those, enable them in file "hardware_language_abstraction.h"
#endif

#ifndef CINTERFACE
#define CINTERFACE

#define TI TreeInterface

#include "tree_interface_config.h"



//TODO POTENTIAL LOSS OF PACKETS : The data size transmitted is not the real size, but the size with BEGIN_BYTES doubled

/*
 * The TreeInterface class is in charge of receiving data from the outside, and to decode them, according to
 *      the syntax specified in "interface_config.h" (More description about the syntax itself is in the config file).
 *
 * When it received a command, it enqueues it in Core's external_tasks queue.
 */

class TreeInterface {

public :

    static void begin();

    static void initialise_aliases();

    static void read_serial();

    //Message emission
    static void send_packet();

    //System commands aliases
    static void echo(String msg);

    static void send_position(float *t);

    //Tree Structure update
    static void send_tree_structure();

    //System packet preparation
    static void prepare_system_packet();

    static void prepare_EEPROM_packet();

private :

    static void prepare_structure_packet();


    //-------------------------------------------------Emission methods-------------------------------------------------
public :
    //Outgoing data reseet;
    static void prepare_data_out(const char *command_id, uint8_t command_id_size);


    //Methods to add data to the current outgoing message

    static void add_char_out(char data);

    static void add_int_out(int data);

    static void add_int32_t_out(int32_t data);

    static void add_float_out(float data);

    static void add_string_out(const char *data);

    //----------------------------------------Emission and Reception data fields----------------------------------------

private:

    /*
     * For input, and output data, a fixed space is allocated, to store the current data.
     *
     * For the emission data, it is structured like the following
     *
     *  0  :  1   :   2    : -> : DATA_SIZE + 1
     * 255 : size :  data    ->
     *
     *
     * And for the reception data, it is structured like the following
     *
     *  0  :  1   :   2    : -> : DATA_SIZE - 1
     * data    ->
     *
     */

    //Emission data : pointer to the end of the message, message origin, and size
    static char *data_out;
    static char *const data_out_0;
    static char *const size_ptr;
    static uint8_t data_out_size;


    //Reception data : pointer to the end of the message, message origin, and size
    static char *data_in;
    static char *const data_in_0;


    static bool packet_began;
    static bool first_detected;
    static uint8_t in_data_remaining;
    static uint8_t in_data_size;


    //Reset incomming data, to receive new packets properly
    static void flush();

    //Incomming message processing
    static void enqueue(char *command, uint8_t size);


    //Command aliases

#define GO_UPPER
#define GO_LOWER(i, name)
#define CREATE_LEAF(i, name)\
    private :\
    static uint8_t name##_size;\
    static char *const name##_id;\
    public :\
    static void send_##name();


#include "tree_interface_config.h"


};

#endif

#endif
