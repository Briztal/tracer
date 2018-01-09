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

#ifndef TRACER_INTERFACES_H
#define TRACER_INTERFACES_H


/*
 * This files simplifies the interface manipulation : it includes the correct files, and defines a macro for the main
 *  interface.
 */

#include <Config/control_config.h>

#ifdef ENABLE_TERMINAL_INTERFACE
#include <Interaction/Languages/Terminal/Terminal.h>
#endif

#ifdef ENABLE_GCODE_INTERFACE
#include <Interaction/Languages/GCode/GCode.h>
#endif

#ifdef ENABLE_PROGRAM_INTERFACE
#include <Communication/ProgramInterface/ProgramInterface.h>
#endif

#include <TaskScheduler/TaskScheduler.h>

#define debug(s) TaskScheduler::log(String("debug ") + s);

#define std_out(s) TaskScheduler::log(s)


class Interaction {

    //------------------------------------------------- Initialisation -------------------------------------------------

public:

    //Initialise all interfaces hardware (data_link);
    static void initialise_hardware();

    //Send the initialisation message on all interfaces
    static void initialisation_message();

    //Initialise all interfaces data (fields and processing environment);
    static void initialise_data();

private:

    //Initialise controllers
    static void initialise_external_controllers();


    //------------------------------------------------- Controls -------------------------------------------------

public:

    //Read internal controllers
    static void read_internal_controllers();

    //Read external controllers;
    static void read_external_controllers();


private:

    //The log_protocol array;
    static Protocol **const protocols;


    //------------------------------------------------- Default log -------------------------------------------------


public:

    static Protocol *get_default_protocol();

    static void *get_default_log_function();

};


//A piece of macro that will help the automatic


#endif //TRACER_INTERFACES_H
