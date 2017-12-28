/*
  TempControl.h - Part of TRACER

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
 * The regulation controller class.
 *
 *  This class manages different regulations of the drone
 */


#ifndef TRACER_CLOSEDLOOPS_H
#define TRACER_CLOSEDLOOPS_H


#include <TaskScheduler/TaskScheduler.h>
#include <ControlLoops/PID.h>


class RegulationController {


    //--------------------------- Data Initialisation ---------------------------

public:

    //Initialise the class in a safe state;
    static void initialise_data();


    //---------------------------Interrupt regulation functions---------------------------

public:

    //The function to initialise_hardware the temperature regulation routine
    static void regulation_init();

    //Temperature regulation routine.
    static void regulation_loop();

    //The function to finalise the temperature regulation routine
    static void regulation_finalisation();


};


#endif //TRACER_CLOSEDLOOPS_H
