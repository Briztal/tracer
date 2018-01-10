/*
  Interfaces.h - Part of TRACER

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

#include "stdint.h"
#include "Interaction.h"
#include <TaskScheduler/TaskScheduler.h>
#include <Interaction/Protocols/ClearText.h>


/*
 * initialise_hardware : this function initialises all transmission layers;
 */

void Interaction::initialise_hardware() {

    //A macro that will solve the interface for a giver control pipeline;
#define EXTERNAL_CONTROL(controller, protocol, buffer, transmission) transmission::begin();

    //Expand the initialise_hardware;
#include <Config/control_config.h>

    //Undef the macro for safety;
#undef EXTERNAL_CONTROL

    delay_ms(200);

}


/*
 * initialisation_message : this function triggers the init_message for all enabled controllers;
 */

void Interaction::initialisation_message() {

    //A macro that will solve the interface for a giver control pipeline;
#define EXTERNAL_CONTROL(controller, protocol, buffer, transmission)\
    controller::init_message();

    //Expand the message;
#include <Config/control_config.h>

    //Undef the macro for safety;
#undef EXTERNAL_CONTROL

}


/*
 * initialise_hardware : this function initialises data for all enabled controllers.
 *
 *  It creates a new Protocol, and gives it to the controller, that will solve its data accordingly;
 */

void Interaction::initialise_data() {

    initialise_external_controllers();

    //TODO initialise_internal_protocols();

}


/*
 * initialise_external_controllers : this function initialises data for external controllers;
 */

void Interaction::initialise_external_controllers() {

    //Cache for the log_protocol;
    Protocol *p;

    //The log_protocol index;
    uint8_t i = 0;


    //A macro that will solve the i-th log_protocol, memorise it, and give it to the controller;
#define EXTERNAL_CONTROL(controller, protocol, buffer, transmission)\
    /*Eventually delete the pointer*/\
    if ((p = protocols[i]) != nullptr) { delete p;};\
    /*Create a new protocol;*/\
    protocols[i++] = p = new protocol((uint8_t)(buffer), transmission::available, transmission::write, transmission::read);\
    /*Initialise the new protocol;*/\
    controller::initialise_data(p);

    //Expand the initialise_hardware;
#include <Config/control_config.h>

    //Undef the macro for safety;
#undef EXTERNAL_CONTROL

}


Protocol *Interaction::get_default_protocol() {

#include <Config/control_config.h>

#ifdef STD_OUT_PROTOCOL

    uint8_t i = 0;

#define EXTERNAL_CONTROL(...) i++;

#include <Config/control_config.h>

#undef EXTERNAL_CONTROL

    if (STD_OUT_PROTOCOL < i)
        return protocols[STD_OUT_PROTOCOL];

#endif

    return nullptr;

}


void *Interaction::get_default_log_function() {

#include <Config/control_config.h>

#ifdef STD_OUT_PROTOCOL

    uint8_t i = STD_OUT_PROTOCOL;

#define EXTERNAL_CONTROL(language, log_protocol, protocol_buffer_size, transmission_layer)\
    if (!(i--)) return (void *) language::external_log;

#include <Config/control_config.h>

#undef EXTERNAL_CONTROL

#endif

    return nullptr;
}


/*
 * read_external_controllers : this function will read_data all interfaces, in order to schedule new tasks.
 *
 *  In order to process all interfaces, with the same priority, the two following rules are applied :
 *
 *      - The function interrogates one interface at the time;
 *
 *      - When being interrogated, an interface schedules at most one task;
 *          This prevents from interrogating the first interface, and fulling the queue, and not being able to
 *          interrogate other interfaces.
 *
 *      - Interaction are interrogated in order, and all with the same priority.
 *          It means that if interface 0 is interrogated, the next interface to be interrogated will
 *          necessarily be interface 1.
 */

void Interaction::read_external_controllers() {

    //The number of interfaces to skip;
    static uint8_t skip_counter = 0;

    //The number of interfaces to skip at the next iteration;
    uint8_t next_skip_counter = 0;

    //A flag, showing if the interrogation must continue (is true if almost one interface can be read_data again);
    bool keep_on;


    /*
     * As the algorithm for querying each interface is exactly the same (but with different values),
     *  we will use a function-like macro.
     */

#define QUERY_INTERFACE(controller, protocol) \
                if (!skip_counter) {\
                    /*Special case of the interface identifier*/\
                    if (!TaskScheduler::available_spaces(255)) {\
                        skip_counter = next_skip_counter;\
                        return;\
                    }\
                    /*Read all data or a packet in the protocol*/\
                    (protocol)->decode_data();\
                    /*Re-iterate the loop if data is still available;*/\
                    if((protocol)->parsing_ready()) {\
                        controller::parse((protocol)->get_data());\
                        keep_on = true;\
                    }\
                /*Do not break, as we must interrogate other interfaces.*/\
                } else skip_counter--;/*If this interface has been skipped, decr the skip_counter.*/\
                /*Another interface to skip if the next one cannot be interrogated*/\
                next_skip_counter++;

    do {

        //Reset the flag. Will be set if one interface has more data available.
        keep_on = false;

        //No interfaces to skip;
        next_skip_counter = 0;

        //Protocol index;
        uint8_t i = 0;

        Protocol *p;

        //Process or switch every controller;
#define EXTERNAL_CONTROL(controller, protocol, ...) \
        p = protocols[i++];\
        QUERY_INTERFACE(controller, p);

        //Expand the processing;
#include <Config/control_config.h>

        //Undef the macro for safety;
#undef EXTERNAL_CONTROL

    } while (keep_on);

}


void Interaction::read_internal_controllers() {


}


Protocol *t_prot_cont[NB_CONTROLS];
Protocol **const Interaction::protocols = t_prot_cont;

