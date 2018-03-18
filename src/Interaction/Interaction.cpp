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

#include "hardware_language_abstraction.h"

#include <Interaction/Delimiter/LineDelimiter.h>


namespace Interaction {

    //---------------------------------------- private fields ----------------------------------------

    //The communication pipes array;
    CommunicationPipe *pipes[NB_PIPES]{nullptr};


    //---------------------------------------- private methods ----------------------------------------

    //Initialise controllers
    static void initialise_communication_pipes();

}


/*
 * initialise_hardware : this function initialises all transmission layers;
 */

void Interaction::initialise_hardware() {

    //A macro that will solve the interface for a giver control pipeline;
#define COMMUNICATION_PIPE(controller, protocol, buffer, transmission) transmission::begin();

    //Expand the initialise_hardware;
#include <Config/control_config.h>

    //Undef the macro for safety;
#undef COMMUNICATION_PIPE

    delay_ms(200);

}


/*
 * initialise_hardware : this function initialises all communication pipes
 */

void Interaction::initialise_data() {

    //Initialise all communication pipes;
    initialise_communication_pipes();

}


/*
 * initialise_communication_pipes : this function initialises data for external controllers;
 */

void Interaction::initialise_communication_pipes() {

    //The pipe index, incremented during the add;
    uint8_t i = 0;

    //A macro that will solve the i-th log_protocol, memorise it, and give it to the controller;
#define COMMUNICATION_PIPE(language, delimiter, buffer, transmission)\
    delete pipes[i];pipes[i++] = new CommunicationPipe(transmission, delimiter(buffer), language());

    //Expand the initialise_hardware;
#include <Config/control_config.h>

    //Undef the macro for safety;
#undef COMMUNICATION_PIPE

}


/*
 * read_communication_pipes : this function read all communication pipes;
 */

void Interaction::read_communication_pipes() {

    //For each communication pipe :
    for (uint8_t pipe_index = 0; pipe_index < NB_PIPES; pipe_index++) {

        //Read all data in the pipe;
        pipes[pipe_index]->readall();

    }

}
