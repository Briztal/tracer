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
 * The temperature controller class.
 *
 *  This class controls the machine's temperature regulations. It is built on the same structure than
 *      MachineController (State Manager, see details in MachineController).
 *
 */


#ifndef TRACER_CLOSEDLOOPS_H
#define TRACER_CLOSEDLOOPS_H


#include <TaskScheduler/TaskScheduler.h>
#include <ControlLoops/PID.h>
#include <ControlLoops/OrientationController.h>

class RegulationController {

    //--------------------------- Data Initialisation ---------------------------

public:

    //Initialise the class in a safe state;
    static void initialise_data();


    //--------------------------- Interrupt regulation functions ---------------------------

public:

    //The function to initialise the control routine
    static void control_init();

    //The control routine.
    static void control_function();


    //--------------------------- Control data ---------------------------

private:

    //The orientation controller;
    static OrientationController orientation_controller;




};


#endif //TRACER_CLOSEDLOOPS_H
