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
#include <ControlSystem/ControlLoops.h>

#include <Kernel/Scheduler/TaskScheduler/TaskScheduler.h>
#include <Kernel/Scheduler/EventManager/EventManager.h>

#include <Kernel/Scheduler/ThreadManager/process.h>

#include "Kernel.h"

#include "Kernel/Arch/arch.h"

namespace Kernel {
    
    //The start flag, set only once, at the entry point. Prevents start to be called a second time.
    bool started = false;

}


//------------------------------------------- Entry Point -------------------------------------------

/*
 * start : this function is called once, by the core initialisation only. It is the project's entry point.
 */

void Kernel::start() {

    //This function can be called only once. If it has already been called, fail.
    if (started)
        return;

    //Set the flag to prevent multiple executions;
    started = true;

    //TODO IN CORE_INIT FUNCTION, CALLED BEFORE KERNEL INIT
    //Initialise the hardware and the framework
    hl_init();

    //Start the thread manager;
    ThreadManager::start();

}


//------------------------------------------- Interaction -------------------------------------------

/*
 * register_communication_pipe : adds the communication pipe to the Interaction data;
 */

void Kernel::register_communication_pipe(CommunicationPipe *pipe) {

    //Add the communication pipe to the interaction data;
    Interaction::add_communication_pipe(pipe);

}


//------------------------------------------- Config checking -------------------------------------------

/*
 * check_config : this function will call _ConfigChecker, and blink the led and display the error message
 *  if it fails;
 */

/*
 * TODO REBUILD

void Kernel::check_config() {

    //Declare the error message;
    tstring error_message = "Error : ";

    //If the config is ok, return;
    if (_ConfigChecker::check_config(error_message)) {
        return;
    }

    const char *m = error_message.data();

    //If the config is not ok :

    //If a led is provided :
#ifdef HL_LED_FLAG
    //Enable the led output;
    pin_mode_output(LED_PIN);
#endif

    //Enable all transmission layers.

    //A macro to enable all interfaces;
#define EXTERNAL_CONTROL(c, p, s, transmission) transmission::begin();

    //Initialise every interface;
#include "Config/control_config.h"

    //Undef the macro for safety;
#undef EXTERNAL_CONTROL

    delay_ms(2000);

    //Infinite loop, as the code is badly configured;
    while(true) {

        //If a led is provided :
#ifdef HL_LED_FLAG
        //Blink the led;
        digital_write(LED_PIN, !digital_read(LED_PIN));
#endif

        //TODO CORRECT SEND
        //A macro to enable all interfaces;
#define EXTERNAL_CONTROL(c, p, s, transmission) transmission::send_str(m);

        //Initialise every interface;
#include "Config/control_config.h"

        //Undef the macro for safety;
#undef EXTERNAL_CONTROL

        //Wait
        delay_ms(1000);

    }

}

  */


//------------------------------------------- Initialisation -------------------------------------------


/*
 * initialise_hardware : this function is called once, by start only. It is the project initialisation function.
 *
 *  As its name suggests, it will initialise_hardware every module of the code.
 */

/*
 * TODO REBUILD
void Kernel::initialise_hardware() {


    //Initialise all enabled interfaces
    Interaction::initialise_hardware();

    //Initialise the SteppersController module only if it is enabled
#ifdef ENABLE_STEPPER_CONTROL

    StepperController::initialise_hardware();

#endif


}
 */


/*
 * initialise_hardware : this function is called once, by start only. It is the project initialisation function.
 *
 *  As its name suggests, it will initialise_hardware every module of the code.
 */

/*
 * TODO REBUILD

void Kernel::initialise_data() {

    //Clear the EEPROMMap profile;
    EEPROMMap::initialise_data();

    //Initialise all enabled interfaces
    Interaction::initialise_data();

    //Initialise the task scheduler;
    TaskScheduler::initialise_data();

    //Initalise the event manager;
    EventManager::reset();

    //Initialise the task programmer;
    TaskProgrammer::reset();


    //Initialise the SteppersController module only if it is enabled
#ifdef ENABLE_STEPPER_CONTROL

    StepperController::initialise_data();

#endif

    //Finally, solve the project;
    Project::initialise_data();

    //Lock the EEPROMMap tree, and solve data across the code if a relevant profile is found in the Storage;
    EEPROMMap::lock_tree();


}
  */

