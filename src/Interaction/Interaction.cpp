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

    //The communication pipes array. Will contain at most 10 pipes;
    DynamicPointerBuffer<CommunicationPipe> pipes(10);


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

        //Initialise the hardware of the pipe;
        pipes.getElement(pipe_index)->initialise_hardware();

    }

    //Delay to avoid freeze after init //TODO REMOVE;
    delay_ms(200);

}


/*
 * initialise_data : this function initialises all communication pipes
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
    for (uint8_t pipe_index = 0; pipe_index < NB_PIPES; pipe_index++) {

        //Read all data in the pipe;
        pipes.getElement(pipe_index)->readall();

    }

}
