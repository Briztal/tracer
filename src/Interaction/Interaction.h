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

#ifndef TRACER_INTERACTION_H
#define TRACER_INTERACTION_H


/*
 * This files simplifies the interface manipulation : it includes the correct files, and defines a macro for the main
 *  interface.
 */

#include <Config/control_config.h>

#include <Kernel/TaskScheduler/TaskScheduler.h>

#include "CommunicationPipe.h"

#define debug(s) TaskScheduler::log(st("debug ") + (s));

#define std_out(s) TaskScheduler::log(s)


namespace Interaction {

    //------------------------------------------------- Initialisation -------------------------------------------------

    //Add a communication pipe;
    void add_communication_pipe(CommunicationPipe *pipe);

    //Initialise all interfaces hardware (data_link);
    void initialise_hardware();

    //Initialise all interfaces data (fields and processing environment);
    void initialise_data();

    //------------------------------------------------- default pipe -------------------------------------------------

    //Get the default communication pipe (the first of the array;
    CommunicationPipe * get_default_pipe();

    //------------------------------------------------- Controls -------------------------------------------------

    //Read all communication pipes;
    void read_communication_pipes();

};


#endif //TRACER_INTERACTION_H
