/*
  Kernel.cpp Part of TRACER

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

#include <Sensors/Thermistors/Thermistors.h>
#include <EEPROM/EEPROMStorage.h>
#include <StepperControl/Steppers.h>
#include <ControlLoops/ControlLoops.h>
#include <TaskScheduler/TaskScheduler.h>
#include <StepperControl/TrajectoryTracer.h>
#include <Control/Control.h>
#include <Sensors/Sensors.h>
#include <Actions/Actions.h>
#include <StepperControl/StepperController.h>
#include "Kernel.h"

//------------------------------------------- Entry Point -------------------------------------------


/*
 * start : this function is called once, by main only. It is the project's entry point.
 *
 *  It will call initialise_hardware, and then call iterate indefinitely.
 */

void Kernel::start() {

    //This function can be called only once. If it has already been called, fail.
    if (started)
        return;

    //Set the flag o prevent multiple excutions;
    started = true;

    //Setup the restoration jump buffer;
    int jump_state = setjmp(restoration_point);

    initialise_hardware();

    //If the restoration point was already used, probabily due to an uncaught exception :
    if (jump_state) {

        delay(500);

        //Log
        std_out("\nAn error has occurred in the code, and it was restored to its initialisation.\n"
                         "You should find an error message in the logs behind. \n"
                         "I hope it will help you to find what has gone wrong ! ");

    }


    //Wait for the eventual retransmission;
    delay(500);

    //Initialise the environment;
    initialise_data();

    //Logo and initialise_hardware message
    Control::initialisation_message();

    //Call the main loop;
    run();


}


//------------------------------------------- Initialisation -------------------------------------------


/*
 * initialise_hardware : this function is called once, by start only. It is the project initialisation function.
 *
 *  As its name suggests, it will initialise_hardware every module of the code.
 */

void Kernel::initialise_hardware() {

    //Initialise the hardware and the framework
    hl_init();

    //Initialise all enabled interfaces
    Control::initialise_hardware();

    //Initialise actions;
    Actions::initialise_hardware();

    //Initialise Sensors
    Sensors::initialise_hardware();

    //Initialise the SteppersController module only if it is enabled
#ifdef ENABLE_STEPPER_CONTROL

    StepperController::initialise_hardware();

#endif

    //TODO MAKE AN INITIALISATION FUNCTION IN ControlLoops
    //Enable the temperature regulation loop
    ControlLoops::enable_temperature();

}


/*
 * initialise_hardware : this function is called once, by start only. It is the project initialisation function.
 *
 *  As its name suggests, it will initialise_hardware every module of the code.
 */

void Kernel::initialise_data() {

    //Initialise all enabled interfaces
    Control::initialise_data();

    //Initialise actions;
    Actions::initialise_data();

    //Initialise Sensors
    Sensors::initialise_data();

    //Initialise the EEPROM data
    EEPROMStorage::initialise_data();


    //Initialise the SteppersController module only if it is enabled
#ifdef ENABLE_STEPPER_CONTROL

    StepperController::initialise_data();

#endif

}


//------------------------------------------- Main Loop -------------------------------------------


/*
 * run : this function is the main loop of the project.
 *
 *  It calls a Scheduler iteration , and eventually executes user defined routines.
 */

void Kernel::run() {

    while (true) {

        //TODO User Routines

        /*
         * Trigger an iteration of the task scheduler :
         *  - read_data all available interfaces until TaskScheduler is full or interface buffers are empties;
         *  - execute all possible tasks in the task pool;
         *  - execute all possible tasks in all sequences;
         */

        TaskScheduler::iterate();


    }

}



//------------------------------------------- Emergency Stop -------------------------------------------


/*
 * emergency_stop : this function stops the code, and long-jumps to the entry point.
 *
 *  Any unhandled exception in the code calls this function, to avoid any unplanned behaviour.
 */

void Kernel::emergency_stop() {

    //Cleat the taskScheduler;
    TaskScheduler::clear();

    //Trigger the TrajectoryTracer's emergency stop function only if the stepper control module is enabled.
#ifdef ENABLE_STEPPER_CONTROL

    TrajectoryTracer::emergency_stop();

#endif

    //Restore TRACER to the initialisation point.
    longjmp(restoration_point, 1);

}


//--------------------------------- Static declarations - definitions ---------------------------------

//The start flag, initialised to false;
bool Kernel::started = false;

//The restoration jump buffer;
jmp_buf Kernel::restoration_point;