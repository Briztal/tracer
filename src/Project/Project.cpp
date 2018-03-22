//
// Created by root on 3/18/18.
//

#include <Project/Interaction/TerminalPipe.h>
#include <Project/Interaction/GCodePipe.h>
#include "Project.h"

#include "MachineController.h"

#include "TemperatureController.h"


/*
 * Initialise the kernel environment;
 */

void Project::initialise_kernel() {

    TerminalPipe::kernel_registration();

}


/*
 * Initialise the project's environment;
 */

void Project::initialise_data() {

    //Initialise the terminal pipe's commands;
    TerminalPipe::initialise_data();

    //Initialise the machine controller;
    MachineController::initialise_data();

    //Initialise the temperature controller;
    TemperatureController::initialise_data();

}