/*
  TerminalInterfaceCommands.cpp - Part of TRACER

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


#include <config.h>

#ifdef ENABLE_TERMINAL_INTERFACE

#include "Interfaces/TerminalInterface/TerminalInterface.h"
#include "TerminalInterfaceCommands.h"
#include "_interface_data.h"
#include <StepperControl/MachineInterface.h>
#include <Actions/ContinuousActions.h>
#include <EEPROM/EEPROMStorage.h>
#include <interface.h>
#include <Project/MachineController.h>
#include <TaskScheduler/TaskScheduler.h>
#include <EEPROM/EEPROMInterface.h>
#include <DataStructures/StringUtils.h>
#include <StepperControl/StepperController.h>
#include <Sensors/Thermistors/Thermistors.h>
#include <Project/TemperatureController.h>


task_state_t TerminalInterfaceCommands::action(uint8_t args_index) {

    EEPROMInterface::display_tree();

    CI::echo("EXIT");

    return complete;

}


//--------------------------------------------------------EEPROM--------------------------------------------------------

task_state_t TerminalInterfaceCommands::eeprom(uint8_t args_index) {

    //Parse Arguments
    PARSE_ARGUMENTS(args_index);

    //verify that f and p arguments are provided.
    REQUIRE_ONE_ARGUMENTS("prd");

    //If the default profile must be reset
    if (CHECK_ARGUMENT('r')) {

        TI::echo("Reseting the EEPROM default profile.");

        //Reset
        EEPROMStorage::set_default_profile();

    }

    //If a path was provided : read or write
    if (CHECK_ARGUMENT('p')) {

        char *path = GET_ARG('p');

        float f;

        //If the value must be written
        if (CHECK_ARGUMENT('w')) {

            //Extract the value to write
            f = GET_ARG_VALUE('w');

            //Log message
            TI::echo("Writing " + String(path) + " to " + String(f));

            //write the variable
            EEPROMInterface::write_data_by_string(path, f);

        }

        //Read and display the value.
        if (EEPROMInterface::read_data_by_string(path, &f)) {

            //Log message
            CI::echo("Value for " + String(path) + " : " + String(f));

        }

    }

    //If the profile must be printed
    if (CHECK_ARGUMENT('d')) {

        //Print the current profile.
        EEPROMInterface::print_stored_data();

    }

    //Succeed
    return complete;

}

//-------------------------------------------------------Movement-------------------------------------------------------

/*
 * IMPORTANT :
 *
 *      All tasks below are tasks of type 0, and must be executed at the order they were received.
 *
 *      We will implement security mechanisms described in the Scheduler help file.
 *
 *
 *      The macro below will be used to verify that [tasks_nb] tasks of type 0 can be executed, and return "reprogram"
 *          if not.
 */

#define FAIL_IF_CANT_SCHEDULE(nb)\
    {\
        bool executable = true;\
        /*This task requires the schedule of 1 task of type 0.*/\
        REQUIRE_SCHEDULE(0, 1, executable);\
        /*If the task is not executable :*/\
        if (!executable) {\
            /*Lock the sequence 0*/\
            LOCK_SEQUENCE(0)\
            /*Schedule a re-execution*/\
            return reprogram;\
        }\
    }\



/*
 * home : this function moves all axis to zero. It takes the following parameters :
 *
 *  -s : homes the machine using endstops.
 *
 */

task_state_t TerminalInterfaceCommands::home(uint8_t args_index) {

    //Require the schedule of one movement
    FAIL_IF_CANT_SCHEDULE(1);

    //Parse arguments
    PARSE_ARGUMENTS(args_index);

    //If the home must use endstops
    if (CHECK_ARGUMENT('e')) {

        CI::echo("ENDSTOPS NOT SUPPORTED FOR INSTANCE");

    } else {
        //If the movement must just be a line to zero

        //Schedule a reset of the carriages
        MachineController::home_scheduled();

    }

    return complete;

}


/*
 * line : this function draws a line to the provided position.
 *  It takes the following arguments  :
 *
 *      -{x, y, z, e} : coordinates of the current carriage. Almost one is required.
 *      - r : (optionnal) all provided coordinates are relative.
 */

task_state_t TerminalInterfaceCommands::line(uint8_t args_index) {

    //This task requires the schedule of one type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Parse Arguments
    PARSE_ARGUMENTS(args_index);

    //verify that almost one movement coordinate is provided.
    REQUIRE_ONE_ARGUMENTS("xyze");

    MachineController::movement_state_t state = MachineController::movement_state_t();

    /*
     * To save repetitive lines of code, a macro that, if a coord is provided, sets its flag and its value.
     */
#define CHECK_COORDINATE(coord, coord_char)\
    if (CHECK_ARGUMENT(coord_char)) {\
        state.coord##_flag = true;\
        state.coord = GET_ARG_VALUE(coord_char);\
    }\

    //If a coordinate was provided, enable this coordinate, and get it.
    CHECK_COORDINATE(x, 'x');
    CHECK_COORDINATE(y, 'y');
    CHECK_COORDINATE(z, 'z');
    CHECK_COORDINATE(e, 'e');

    //For safety, un-define the previously created macro.
#undef CHECK_COORDINATE

    //If the movement is absolute, move to the destination
    if (CHECK_ARGUMENT('r')) {

        //Mark the movement to be relative.
        state.relative_flag = true;

    }

    //Schedule a line to the specified coordinates
    return MachineController::line_scheduled(state);

}


//--------------------------------------------------------Extrusion-----------------------------------------------------

/*
 * enable_steppers : enable or disable steppers.
 *
 *  It takes only one argument, -e followed by 0 (disable) or [not zero] enabled
 *
 */

task_state_t TerminalInterfaceCommands::enable_steppers(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Parse Arguments
    PARSE_ARGUMENTS(args_index);

    //Fail if the enabling argument is omitted
    REQUIRE_ALL_ARGUMENTS("e")

    //Extract the enable boolean
    bool enable = (bool) GET_ARG_VALUE('e');

    //Schedule an enable / disable of steppers.
    return MachineController::enable_steppers_scheduled(enable);

}


/*
 * set_extrusion : this function modifies extrusion parameters.
 *
 *  It takes the folloging arguments :
 *      - c : changes the working carriage;
 *      - s : changes the speed for the carriages designed by c (or for the working carriage if c is not provided).
 *
 *  Almost one must be provided.
 *
 */
task_state_t TerminalInterfaceCommands::set_extrusion(uint8_t args_index) {

    FAIL_IF_CANT_SCHEDULE(1);

    //Parse Arguments
    PARSE_ARGUMENTS(args_index);

    //At least w (working carriage) and s (speed) must be provided
    REQUIRE_ONE_ARGUMENTS("cs");

    MachineController::extrusion_state_t new_state = MachineController::extrusion_state_t();

    //If the working carriage must be changed
    if (CHECK_ARGUMENT('c')) {

        //Set the flag;
        new_state.working_extruder_flag = true;

        //Set the new working extruder
        new_state.working_extruder = (uint8_t) GET_ARG_VALUE('w');

    }

    //If the speed must be changed
    if (CHECK_ARGUMENT('s')) {

        //If we must set the speed for a particular carriage
        if (CHECK_ARGUMENT('c')) {

            /* As we must select the right speed group, and the process is repetitive, we will use a macro.
             *  that check for the extruder id, sets the appropriate flag and speed value.
             */

#define CHECK_SPEED(i) \
            case(i): new_state.speed_##i##_flag = true;new_state.speed_##i = GET_ARG_VALUE('s');break;

            switch ((uint8_t) GET_ARG_VALUE('c')) {
                CHECK_SPEED(0)
                CHECK_SPEED(1)
                CHECK_SPEED(2)
                CHECK_SPEED(3)
                default:
                    break;

            }

            //for safety, un-delete the previously defined macro.
#undef CHECK_SPEED

        } else {
            //If we must set the speed for the current carriage

            //Set the flag;
            new_state.current_speed_flag = true;

            //Set the new working extruder
            new_state.current_speed = GET_ARG_VALUE('s');
        }

    }

    return MachineController::set_extrusion_state(new_state);;

}


/*
 * set_cooling : this function modifies the cooling state.
 *
 *  It takes the following arguments :
 *      -e : 0 means disable the cooling, other values will enable it.
 *      -p : modifies the cooling power (truncated between 0 and 100).
 *
 */

task_state_t TerminalInterfaceCommands::set_cooling(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Parse Arguments
    PARSE_ARGUMENTS(args_index)

    //Fail if none of power of enable are provided
    REQUIRE_ONE_ARGUMENTS("ep");

    MachineController::cooling_state_t new_state = MachineController::cooling_state_t();

    //Set the enable state if required
    if (CHECK_ARGUMENT('e')) {

        //Set the flag
        new_state.enabled_flag = true;

        //get the enable boolean
        new_state.enabled = (bool) GET_ARG_VALUE('e');

    }

    //Set the power if required
    if (CHECK_ARGUMENT('p')) {

        //Set the flag
        new_state.enabled_flag = true;

        //Set the new power
        new_state.power = GET_ARG_VALUE('p');

    }

    return MachineController::set_cooling_state(new_state);;

}


/*
 * set_hotend : this function sets the state of a particular hotend.
 *
 *  It takes the following arguments :
 *      -h : the hotend to modify, mandatory.
 *      -e : 0 means disable the power on the hotend, other values will enable it.
 *      -t : sets the target temperature of the hotend.
 *
 *      -e or -t must be provided.
 */

task_state_t TerminalInterfaceCommands::set_hotend(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Parse Arguments
    PARSE_ARGUMENTS(args_index)

    //Fail if a hotend was not specified
    REQUIRE_ALL_ARGUMENTS("h")

    //Fail if neither temperature (t) or enable_state (e) are provided.
    REQUIRE_ONE_ARGUMENTS("te");
    TemperatureController::hotend_state_t state = TemperatureController::hotend_state_t();

    state.hotend_flag = true;
    state.hotend = (uint8_t) GET_ARG_VALUE('h');

    //If the temperature must be adjusted
    if (CHECK_ARGUMENT('t')) {

        //Set the temperature flag
        state.temperature_flag = true;

        //Get the temperature
        state.temperature = GET_ARG_VALUE('t');

    }

    //If the activity must be changed
    if (CHECK_ARGUMENT('e')) {

        //Set the enable flag
        state.enabled_flag = true;

        //Get the temperature
        state.enabled = (bool) GET_ARG_VALUE('e');

    }

    //Schedule an enable/disable of the hotbed regulation.
    return TemperatureController::set_hotends_state(state);

}


/*
 * set_hotbed : this function sets the hotbed's state
 *
 *  It takes the following arguments :
 *      -e : 0 means disable the power on the hotbed, other values will enable it.
 *      -t : sets the target temperature of the hotbed.
 *
 *      -e or -t must be provided.
 */

task_state_t TerminalInterfaceCommands::set_hotbed(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Parse Arguments
    PARSE_ARGUMENTS(args_index)

    //Fail if neither temperature (t) or enable_state (e) are provided.
    REQUIRE_ONE_ARGUMENTS("te");
    TemperatureController::hotbed_state_t state = TemperatureController::hotbed_state_t();


    //If the temperature must be adjusted
    if (CHECK_ARGUMENT('t')) {

        //Set the temperature flag
        state.temperature_flag = true;

        //Get the temperature
        state.temperature = GET_ARG_VALUE('t');

    }

    //If the activity must be changed
    if (CHECK_ARGUMENT('e')) {

        //Set the enable flag
        state.enabled_flag = true;

        //Get the temperature
        state.enabled = (bool) GET_ARG_VALUE('e');

    }

    //Schedule an enable/disable of the hotbed regulation.
    return TemperatureController::set_hotbed_state(state);

}




//---------------------------------------------------------Gets---------------------------------------------------------



task_state_t TerminalInterfaceCommands::get_regulations(uint8_t args_index) {

    return complete;

}


task_state_t TerminalInterfaceCommands::get_temps(uint8_t args_index) {

    return complete;

}



//---------------------------------------------------------Tests--------------------------------------------------------


task_state_t TerminalInterfaceCommands::stepper_test(uint8_t args_index) {

    CI::echo("EXIT");

    return complete;


}


task_state_t TerminalInterfaceCommands::temp_test(uint8_t args_index) {


    CI::echo("t0 : " + String(Thermistors::get_temperature_hotend_0(845), 5));
    CI::echo("t0 : " + String(Thermistors::get_temperature_hotend_0(846), 5));
    CI::echo("t0 : " + String(Thermistors::get_temperature_hotend_0(847), 5));
    CI::echo("t0 : " + String(Thermistors::get_temperature_hotend_0(846), 5));
    CI::echo("t0 : " + String(Thermistors::get_temperature_hotend_0(845), 5));


    return complete;
}

char t_wd_buf_tic[MAX_WORD_SIZE];
char *const TerminalInterfaceCommands::word_buffer = t_wd_buf_tic;

#endif

