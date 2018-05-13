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


namespace Interaction {

    //---------------------------------------- private fields ----------------------------------------

    //The communication pipes array. Will contain at most 10 pipes;
    DynamicPointerBuffer<CommunicationPipe> pipes(10);


    //------------------------------------------ Log -----------------------------------------

    //The communication log_protocol;
    static CommunicationPipe *log_pipe;

}


/*
 * add_communication_pipe : adds the given communication pipe to the array;
 */

void Interaction::add_communication_pipe(CommunicationPipe *pipe) {

    //Add the pipe to the array;
    pipes.add(pipe);

}


/*
 * initialise_hardware : this function initialises all transmission layers;
 */

void Interaction::initialise_hardware() {

    //For each pipe :
    for (uint8_t pipe_index = pipes.getSize(); pipe_index--;) {

        //Cache the pipe;
        CommunicationPipe *pipe = pipes.getElement(pipe_index);

        //Set it as the current log pipe (so that it can use std_out);
        setCommunicationPipe(*pipe);

        //Initialise the hardware of the pipe;
        pipes.getElement(pipe_index)->initialise_hardware();

    }

    //At the end, the current communication pipe is the first one. It will be the default one;

    //Delay to avoid freeze after init //TODO REMOVE;
    delay_ms(200);

}


/*
 * reset : this function initialises all communication pipes
 */

void Interaction::initialise_data() {

}


//------------------------------------------------- default pipe -------------------------------------------------

/*
 * get_default_pipe : returns the first pipe of the array;
 */

CommunicationPipe * Interaction::get_default_pipe() {

    //If no pipes have been added, return nullptr;
    if (!pipes.getSize()) {
        return nullptr;
    }

    //Return the first element of the pipe array;
    return pipes.getElement(0);

}

//------------------------------------------------- Controls -------------------------------------------------

/*
 * read_communication_pipes : this function read all communication pipes;
 */

void Interaction::read_communication_pipes() {


    //For each communication pipe :
    for (uint8_t pipe_index = NB_PIPES; pipe_index --; ) {

        //Cache the pipe;
        CommunicationPipe *pipe = pipes.getElement(pipe_index);

        //Set it as the current log pipe (so that it can use std_out);
        setCommunicationPipe(*pipe);

        //Read all data in the pipe;
        pipe->readall();

    }

    //At the end, the current communication pipe is the first one. It will be the default one;

}



//-------------------------------------------------------- Log ---------------------------------------------------------


void Interaction::log(tstring &message) {

    //If the log pipe is not null;
    if (log_pipe) {

        //Send the message through the log_pipe;
        log_pipe->send(message, 0);

    }

}

void Interaction::log(tstring &&message) {

    //If the log pipe is not null;
    if (log_pipe) {

        //Send the message through the log_pipe;
        log_pipe->send(message, 0);

    }
}


//------------------------------------------ Communication pipe set -----------------------------------------


/*
 * getCommunicationPipe : returns the current communication pipe;
 */

CommunicationPipe* Interaction::getCommunicationPipe() {

    //Return the current communication pipe;
    return log_pipe;

}


/*
 * setCommunicationPipe : sets the provided communication pipe as the log pipe;
 */

void Interaction::setCommunicationPipe(CommunicationPipe &pipe) {

    //Set the log pipe;
    log_pipe = &pipe;

}
