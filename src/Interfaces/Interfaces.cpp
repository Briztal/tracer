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
#include "Interfaces.h"
#include <TaskScheduler/TaskScheduler.h>

void Interfaces::initialise_hardware() {

#ifdef ENABLE_GCODE_INTERFACE
    GI::initialise_hardware();
#endif

#ifdef ENABLE_PROGRAM_INTERFACE
    PI::initialise_hardware();
#endif

#ifdef ENABLE_TERMINAL_INTERFACE
    TI::initialise_hardware();
#endif

}


void Interfaces::initialisation_message() {

#ifdef ENABLE_GCODE_INTERFACE
    GI::init_message();
#endif

#ifdef ENABLE_PROGRAM_INTERFACE
    PI::init_message();
#endif

#ifdef ENABLE_TERMINAL_INTERFACE
    TI::init_message();
#endif

}


void Interfaces::initialise_data() {

#ifdef ENABLE_GCODE_INTERFACE
    GI::initialise_data();
#endif

#ifdef ENABLE_PROGRAM_INTERFACE
    PI::initialise_data();
#endif

#ifdef ENABLE_TERMINAL_INTERFACE
    TI::initialise_data();
#endif

}


/*
 * read_interfaces : this function will read all interfaces, in order to schedule new tasks.
 *
 *  In order to process all interfaces, with the same priority, the two following rules are applied :
 *
 *      - The function interrogates one interface at the time;
 *
 *      - When being interrogated, an interface schedules at most one task;
 *          This prevents from interrogating the first interface, and fulling the queue, and not being able to
 *          interrogate other interfaces.
 *
 *      - Interfaces are interrogated in order, and all with the same priority.
 *          It means that if interface 0 is interrogated, the next interface to be interrogated will
 *          necessarily be interface 1.
 */

void Interfaces::read_interfaces() {

    //The number of interfaces to skip;
    static uint8_t skip_counter = 0;

    //The number of interfaces to skip at the next iteration;
    uint8_t next_skip_counter = 0;

    //A flag, showing if the interrogation must continue (is true if almost one interface can be read again);
    bool keep_on;

    /*
     * As the algorithm for querying each interface is exactly the same (but with different values,
     *  we will use a function-like macro.
     */

#define QUERY_INTERFACE(interface_name) \
                if (!skip_counter) {\
                    /*Special case of the interface identifier*/\
                    if (!TaskScheduler::available_spaces(255)) {\
                        skip_counter = next_skip_counter;\
                        return;\
                    }\
                    /*Re-iterate the loop if data is still available;*/\
                    keep_on |= interface_name::read_data();\
                    /*Do not break, as we must interrogate other interfaces.*/\
                } else skip_counter--;/*If this interface has been skipped, decr the skip_counter.*/\
                /*Another interface to skip if the next one cannot be interrogated*/\
                next_skip_counter++;

    do {

        //Reset the flag. Will be set if one interface has more data available.
        keep_on = false;

        //No interfaces to skip;
        next_skip_counter = 0;


#ifdef ENABLE_TERMINAL_INTERFACE
        QUERY_INTERFACE(TI)
#endif


#ifdef ENABLE_GCODE_INTERFACE
        QUERY_INTERFACE(GI)
#endif


#ifdef ENABLE_PROGRAM_INTERFACE
        QUERY_INTERFACE(PI)
#endif

    } while(keep_on);

}

