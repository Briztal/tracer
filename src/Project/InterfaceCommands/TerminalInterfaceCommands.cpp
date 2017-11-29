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

task_state_t TerminalInterfaceCommands::eeprom_write(uint8_t args_index) {

    //Parse Arguments
    PARSE_ARGUMENTS(args_index);

    CI::echo("SUUS");

    //verify that f and p arguments are provided.
    REQUIRE_ALL_ARGUMENTS("vp");

    //Extract the value to write
    float f = GET_ARG_VALUE('v');

    char *path = GET_ARG('p');

    EEPROMInterface::write_data_by_string(path, f);

    CI::echo("EXIT " + String(f));

    return complete;

}


task_state_t TerminalInterfaceCommands::eeprom_read(uint8_t args_index) {

    //Parse Arguments
    PARSE_ARGUMENTS(args_index);

    //verify that f and p arguments are provided.
    REQUIRE_ALL_ARGUMENTS("p");

    //Get the path
    char *path = GET_ARG('p');

    //Initialise the float that will contain the reading value.
    float f;

    //Read and display the value.
    if (EEPROMInterface::read_data_by_string(path, &f)) {
        CI::echo("value : " + String(f));
    }

    return complete;

}


task_state_t TerminalInterfaceCommands::eeprom_reset(uint8_t args_index) {

    EEPROMStorage::set_default_profile();

    return complete;

}


task_state_t TerminalInterfaceCommands::eeprom_print(uint8_t args_index) {

    EEPROMInterface::print_stored_data();

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


task_state_t TerminalInterfaceCommands::move_home(uint8_t args_index) {

    CI::echo("ENDSTOPS NOT SUPPORTED FOR INSTANCE");

    return complete;

}


task_state_t TerminalInterfaceCommands::move_zero(uint8_t args_index) {

    //This task requires the schedule of one type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Schedule a reset of the carriages
    MachineController::carriages_reset_scheduled();

    return complete;


}


task_state_t TerminalInterfaceCommands::line_to(uint8_t args_index) {

    //This task requires the schedule of one type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Parse Arguments
    PARSE_ARGUMENTS(args_index);

    //verify that almost one movement coordinate is provided.
    REQUIRE_ONE_ARGUMENTS("xyze");

    machine_coords_t t = machine_coords_t();

    //If x was provided, enable the x coordinate, and get it.
    if (CHECK_ARGUMENT('x')) {
        t.x_enabled = true;
        t.x = GET_ARG_VALUE('x');
    }

    if (CHECK_ARGUMENT('y')) {
        t.y_enabled = true;
        t.y = GET_ARG_VALUE('y');
    }

    if (CHECK_ARGUMENT('z')) {
        t.z_enabled = true;
        t.z = GET_ARG_VALUE('z');
    }

    if (CHECK_ARGUMENT('e')) {
        t.e_enabled = true;
        t.e = GET_ARG_VALUE('e');
    }


    //Schedule a line to the specified coordinates
    return MachineController::line_to_scheduled(t);

}


task_state_t TerminalInterfaceCommands::line_of(uint8_t args_index) {

    //This task requires the schedule of one type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Parse Arguments
    PARSE_ARGUMENTS(args_index);

    //verify that almost one movement coordinate is provided.
    REQUIRE_ONE_ARGUMENTS("xyze");

    machine_coords_t t = machine_coords_t();

    //If x was provided, enable the x coordinate, and get it.
    if (CHECK_ARGUMENT('x')) {
        t.x_enabled = true;
        t.x = GET_ARG_VALUE('x');
    }

    if (CHECK_ARGUMENT('y')) {
        t.y_enabled = true;
        t.y = GET_ARG_VALUE('y');
    }

    if (CHECK_ARGUMENT('z')) {
        t.z_enabled = true;
        t.z = GET_ARG_VALUE('z');
    }

    if (CHECK_ARGUMENT('e')) {
        t.e_enabled = true;
        t.e = GET_ARG_VALUE('e');
    }

    //Schedule a line to the specified coordinates with the specified working_extruder
    return MachineController::line_of_scheduled(t);

}

//--------------------------------------------------------Extrusion-----------------------------------------------------


task_state_t TerminalInterfaceCommands::enable_steppers(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Parse Arguments
    PARSE_ARGUMENTS(args_index);

    //Fail if the enabling argument is omitted
    REQUIRE_ALL_ARGUMENTS("e")

    //TODO SELECT THE STEPPER TO DISABLE, WITH ARGUMENT -s

    //Extract the enable boolean
    bool enable = (bool) GET_ARG_VALUE('e');

    //Schedule an enable / disable of steppers.
    return MachineController::enable_steppers_scheduled(enable);

}



task_state_t TerminalInterfaceCommands::set_extrusion(uint8_t args_index) {

    FAIL_IF_CANT_SCHEDULE(1);

    //Parse Arguments
    PARSE_ARGUMENTS(args_index);

    //At least w (working carriage) and s (speed) must be provided
    REQUIRE_ONE_ARGUMENTS("ws");

    MachineController::extrusion_state_t new_state = MachineController::extrusion_state_t();

    //If the working carriage must be changed
    if (CHECK_ARGUMENT('w')) {

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

            switch((uint8_t) GET_ARG_VALUE('c')) {
                CHECK_SPEED(0)
                CHECK_SPEED(1)
                CHECK_SPEED(2)
                CHECK_SPEED(3)
                default:break;

            }

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
    state.hotend = (uint8_t)GET_ARG_VALUE('h');

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




    CI::echo("t0 : "+String(Thermistors::get_temperature_hotend_0(845), 5));
    CI::echo("t0 : "+String(Thermistors::get_temperature_hotend_0(846), 5));
    CI::echo("t0 : "+String(Thermistors::get_temperature_hotend_0(847), 5));
    CI::echo("t0 : "+String(Thermistors::get_temperature_hotend_0(846), 5));
    CI::echo("t0 : "+String(Thermistors::get_temperature_hotend_0(845), 5));




    return complete;
}

char t_wd_buf_tic[MAX_WORD_SIZE];
char *const TerminalInterfaceCommands::word_buffer = t_wd_buf_tic;

#endif

