/*
  MovementCoordinator.h - Part of TRACER

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


/*
 * MovementCoordinator : this class is the authority that manages the access to the movement routine;
 *
 *  When a function needs to execute a function in the stepper routine, it requires the schedule to this class,
 *      which authorises it or not, whether the access is already reserved or not;
 */

#ifndef TRACER_MOVEMENTCOORDINATOR_H
#define TRACER_MOVEMENTCOORDINATOR_H

#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL

#include "hardware_language_abstraction.h"

class MovementCoordinator {

public:

    //Return the status of the routine;
    static inline bool started() {
        return started_flag;
    }

    //Enable the interrupt;
    static inline void enable_interrupt() {

        //Start the interrupt sequence, only if the routine is started;
        if (started_flag) {
            enable_stepper_interrupt();
        }

    }

    //Disable the interrupt;
    static inline void disable_interrupt() {
        disable_stepper_interrupt();
    }

    static inline void set_interrupt_period(float period) {
        set_stepper_int_period(period);
    }

    static inline void set_interrupt_function(void (*f)()) {
        //Start the interrupt sequence, only if the routine is started;
        set_stepper_int_function(f);
    }

    static bool reserve();

    static void stop();

private :

    //The state of the movement routine. Public because accessed by other modules.
    static volatile bool started_flag;



};


#endif //TRACER_MOVEMENTCOORDINATOR_H

#endif