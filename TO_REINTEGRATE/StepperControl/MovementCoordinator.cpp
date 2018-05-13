/*
  MovementCoordinator.cpp - Part of TRACER

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


#include "MovementCoordinator.h"

/*
 * reserve : this function is called by another process_t to reserve the usage of the routine;
 */

bool MovementCoordinator::reserve() {

    //If the routine is already in use :
    if (started_flag) {

        //Fail;
        return false;

    } else {

        //mark the routine started;
        started_flag = true;

        //Start the timer;
        enable_stepper_timer();

        //Complete
        return true;
    }
}


/*
 * stop : this function stops the routine (interrupt + timer), and saves the stopped state;
 */
void MovementCoordinator::stop() {

    //Reset the start flag, so that no more interrupt is programmed;
    started_flag = false;

    //Disable the interrupt;
    disable_stepper_interrupt();

    //Disable the timer;
    disable_stepper_timer();

}


//--------------------------------------- Static declarations - definitions ---------------------------------------

volatile bool MovementCoordinator::started_flag = false;