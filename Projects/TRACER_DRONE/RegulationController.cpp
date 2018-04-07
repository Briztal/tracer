/*
  TempControl.cpp - Part of TRACER

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


#include "RegulationController.h"

#include "ControlLoops/ControlLoops.h"


//----------------------------------------------------- Data init ------------------------------------------------------


/*
 * initialise_data : this function initialises data in a safe state, and registers PID in the Storage;
 */

void RegulationController::initialise_data() {

    //initialise

    orientation_controller = OrientationController();

    //Initialise the regulation;
    control_init();

    //Start the regulation loop;
    Loop0::start(control_function, 10);

}


//-----------------------------------------------------Regulation-------------------------------------------------------


/*
 * control_init : this function is called when the temperature regulation is started_flag.
 *
 *  It resets all PIDs.
 *
 */

void RegulationController::control_init() {

    //Reset the orientation controller;
    orientation_controller.reset();

}


/*
 * control_function : this function is the interrupt routine, called to regulate temperatures in the machine.
 *
 *  It checks if each thermistor is enabled, and if it is, regulated its temperature.
 *
 */

void RegulationController::control_function() {

    //TODO GET DATA FROM SENSOR

    //TODO TRANSLATE TO ORIENTATION

    //TODO COMPUTE THE CORRECTION VECTOR;


    //TODO GET HEIGHT / POWER FROM SENSOR / COMMAND;


    //TODO COMPUTE AND APPLY POWERS

};



//-------------------------------------------Static declaration / definitions-------------------------------------------

#define m RegulationController

OrientationController m::orientation_controller();