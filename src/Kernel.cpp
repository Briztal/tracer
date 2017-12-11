//
// Created by root on 09/12/17.
//

#include <Sensors/Thermistors/Thermistors.h>
#include <EEPROM/EEPROMStorage.h>
#include <StepperControl/StepperController.h>
#include <ControlLoops/ControlLoops.h>
#include <TaskScheduler/TaskScheduler.h>
#include <StepperControl/TrajectoryTracer.h>
#include "Kernel.h"

//------------------------------------------- Entry Point -------------------------------------------


/*
 * start : this function is called once, by main only. It is the project's entry point.
 *
 *  It will call initialise_data, and then call iterate indefinitely.
 */

void Kernel::start() {

    //This function can be called only once. If it has already been called, fail.
    if (started)
        return;

    //Set the flag o prevent multiple excutions;
    started = true;

    //Setup the restoration jump buffer;
    int jump_state = setjmp(restoration_point);

    initialise();

    //If the restoration point was already used, probabily due to an uncaught exception :
    if (jump_state) {

        delay(500);

        //Log
        CI::echo("\nAn error has occurred in the code, and it was restored to its initialisation.\n"
                         "You should find an error message in the logs behind. \n"
                         "I hope it will help you to find what has gone wrong ! ");

    }


    //Logo and initialise_data message
    display_logo();


    //Call the main loop;
    run();


}


//------------------------------------------- Initialisation -------------------------------------------


/*
 * initialise : this function is called once, by start only. It is the project initialisation function.
 *
 *  As its name suggests, it will initialise_data every module of the code.
 */

void Kernel::initialise() {

    //Initialise the hardware and the framework
    hl_init();

    //Initialise all enabled interfaces
    initialise_interfaces();

    //Initialise Thermistors
    Thermistors::init();

    //Initialise the EEPROM data
    EEPROMStorage::init();

    //Initialise the StepperController module only if it is enabled
#ifdef ENABLE_STEPPER_CONTROL

    StepperController::init();

#endif

    //TODO MAKE AN INITIALISATION FUNCTION IN ControlLoops
    //Enable the temperature regulation loop
    ControlLoops::enable_temperature();

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
         *  - read all available interfaces until TaskScheduler is full or interface buffers are empties;
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

//------------------------------------------- Logo Display -------------------------------------------

/*
 * The following function displays a logo at the initialisation of the code.
 *
 * //TODO I'll add some macros, to accelerate the version / author customisation.
 */

void Kernel::display_logo() {

    CI::echo("");
    CI::echo("    dBBBBBBP  dBBBBb  dBBBBb     dBBBP  dBBBP  dBBBBb");
    CI::echo("      dBP     dP dBP   BBABB    dBP    dBP     BP dBP");
    CI::echo("     dBP     dBBBBK'  dBP BB   dBP    dBBP    dBBBBK'");
    CI::echo("    dBP     dBP  BB  dBBBBBB  dBP    dBP     dBP  BB");
    CI::echo("   dBP     dBP  dB' dBP   BB dBBBBP dBBBBP  dBP  dB'");
    CI::echo("");
    CI::echo("\nTRACER initialised and ready. Waiting for commands.\n\n");

}

//--------------------------------- Static declarations - definitions ---------------------------------

//The start flag, initialised to false;
bool Kernel::started = false;

//The restoration jump buffer;
jmp_buf Kernel::restoration_point;