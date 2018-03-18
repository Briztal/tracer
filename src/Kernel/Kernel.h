/*
  Kernel.h Part of TRACER

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

#ifndef TRACER_KERNEL_H
#define TRACER_KERNEL_H

#include <sanity_check.h>

#include <Interaction/CommunicationPipe.h>

#include "setjmp.h"

namespace Kernel {


    //------------------------------------------- Entry Point -------------------------------------------

    /*
     * start : this function is called once, by main only. It is the project's entry point.
     *
     *  It will call initialise_hardware, and then call iterate indefinitely.
     */

    void start();


    //------------------------------------------- Interaction -------------------------------------------

    /*
     * register_communication_pipe : registers the given communication pipe to the interaction pocedure;
     */

    void register_communication_pipe(CommunicationPipe *pipe);

    
    //------------------------------------------- Emergency Stop -------------------------------------------

    /*
     * emergency_stop : this function stops the code, and long-jumps to the entry point.
     *
     *  Any unhandled exception in the code calls this function, to avoid any unplanned behaviour.
     */

    void emergency_stop();

};


#endif //TRACER_KERNEL_H
