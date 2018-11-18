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
 * This files simplifies the if manipulation : it includes the correct files, and defines a macro for the main
 *  if.
 */

#include <Config/control_config.h>

#include <Kernel/Scheduler/TaskScheduler/TaskScheduler.h>

#include "CommunicationPipe.h"

#define debug(s) Interaction::log(st("debug ") + (s));

#define std_out(s) Interaction::log(s)


namespace Interaction {

    //------------------------------------------------- Initialisation -------------------------------------------------

    //Add a interaction pipe;
    void add_communication_pipe(CommunicationPipe *pipe);

    //Initialise all interfaces hardware (data_link);
    void initialise_hardware();

    //Initialise all interfaces data (fields and processing environment);
    void initialise_data();

    //------------------------------------------------- default pipe -------------------------------------------------

    //Get the default interaction pipe (the first of the array;
    CommunicationPipe * get_default_pipe();

    //------------------------------------------------- Controls -------------------------------------------------

    //Read all interaction pipes;
    void read_communication_pipes();


    //------------------------------------------ Log -----------------------------------------

    //Echo a message on the current log pipe;
    void log(tstring &message);

    //Echo a message on the current log pipe;
    void log(tstring &&message);


    //------------------------------------------ Communication pipe set -----------------------------------------

    //Get a reference to the current interaction pipe;
    CommunicationPipe* getCommunicationPipe();

    //Set the interaction pipe to a given one;
    void setCommunicationPipe(CommunicationPipe &pipe);

};


#endif //TRACER_INTERACTION_H
