/*
  GCodeInterfaceCommands.cpp - Part of TRACER

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


#include <Config/control_config.h>

#ifdef ENABLE_GCODE_INTERFACE

#include <Interaction/Interaction.h>

#include "GCodePipe.h"

#include <Project/MachineController.h>
#include <Project/TemperatureController.h>
#include <Sensors/Thermistors/Thermistors.h>
#include <Interaction/Language/Languages/GCodeLanguage.h>
#include <Kernel/Kernel.h>
#include <Interaction/Delimiter/LineDelimiter.h>

#include "hardware_language_abstraction.h"

namespace GCodePipe {


    task_state_t home();
    task_state_t line();

    task_state_t enable_steppers();
    task_state_t set_position();
    task_state_t set_extrusion();
    task_state_t set_cooling();
    task_state_t set_hotend();
    task_state_t set_hotbed();
    
    task_state_t get_temps();
    task_state_t get_regulations();
    
    task_state_t stepper_test();
    task_state_t temp_test();
    task_state_t action();

    task_state_t eeprom();
    //The language we will use;
    GCodeLanguage * language;

};


/*
 * This function creates a communication pipe and registers it in the kernel;
 */

void GCodePipe::kernel_registration() {

    //Instantiate our language;
    language = new GCodeLanguage(15, 10);

    //Instantiate our pipe;
    CommunicationPipe *pipe = new CommunicationPipe(Serial, new LineDelimiter(100), language);

    //Register the pipe to the kernel;
    Kernel::register_communication_pipe(pipe);

}

/*
 * This function creates the language's tree;
 */

void GCodePipe::initialise_data() {
    
    language->clear();

    //Movement
    language->addCommand("G00", home);
    language->addCommand("G01", line);

    //Setup
    language->addCommand("G10", enable_steppers);
    language->addCommand("G11", set_position);
    language->addCommand("G12", set_extrusion);
    language->addCommand("G13", set_cooling);
    language->addCommand("G14", set_hotend);
    language->addCommand("G15", set_hotbed);

    //Get
    language->addCommand("G20", get_temps);
    language->addCommand("G21", get_regulations);

    //test
    language->addCommand("G30", stepper_test);
    language->addCommand("G31", temp_test);
    language->addCommand("G32", action);

    //EEPROM
    language->addCommand("G4", eeprom);
            
}



#define PARSE_ARGUMENTS(arguments) {if (!language->parse_arguments(arguments)) return invalid_arguments;}

#define REQUIRE_ALL_ARGUMENTS(arg_string) {if (!language->verify_all_identifiers_presence(arg_string)) return invalid_arguments;}

#define REQUIRE_ONE_ARGUMENTS(arg_string) {if (!language->verify_one_identifiers_presence(arg_string)) return invalid_arguments;}

#define CHECK_ARGUMENT(identifier) (language->verify_identifier_presence(identifier))

#define GET_ARG(identifier) language->get_argument(identifier)

#define GET_ARG_VALUE(identifier) language->get_argument_value(identifier)


task_state_t GCodePipe::action() {

    std_out("DUMB COMMAND");

    return complete;

}


//--------------------------------------------------------EEPROM--------------------------------------------------------

task_state_t GCodePipe::eeprom() {

    //verify that function and p content are provided.
    REQUIRE_ONE_ARGUMENTS("PRD");

    //If the default profile must be reset
    if (CHECK_ARGUMENT('R')) {

        std_out("Reseting the EEPROM default profile.");

        //Reset
        //EEPROMStorage::set_default_profile();

    }

    //If a path was provided : read_data or write_data
    if (CHECK_ARGUMENT('P')) {

        const char *path = GET_ARG('P');

        float f;

        //If the value must be written
        if (CHECK_ARGUMENT('W')) {

            //Extract the value to write_data
            f = GET_ARG_VALUE('W');

            //Log message
            std_out(string("Writing ") + path + " to " + f);

            //write_data the variable
            //EEPROMInterface::write_data_by_string(path, f);

        }

        /*
        //Read and display the value.
        if (EEPROMInterface::read_data_by_string(path, &f)) {

            //Log message
            std_out("Value for " + string(path) + " : " + string(f));

        }*/

    }

    //If the profile must be printed
    if (CHECK_ARGUMENT('D')) {

        //Print the current profile.
        //EEPROMInterface::print_stored_data();

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
        /*If the task is not executable :*/\
        if (!TaskScheduler::verify_schedulability(0, nb)) {\
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

task_state_t GCodePipe::home() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare the structure;
    MachineController::home_state_t state;

    //If the home must use endstops
    if (CHECK_ARGUMENT('E')) {

        std_out("ENDSTOPS NOT SUPPORTED FOR INSTANCE");

    } else {
        //If the movement must just be a line to zero

        //Reset the flag;
        state.endstops_flag = false;

        //Schedule a reset of the carriages
        MachineController::home_scheduled_0(state);

    }

    return complete;

}


/*
 * line : this function draws a line to the provided position.
 *  It takes the following content  :
 *
 *      -{x, y, z, e} : coordinates of the current carriage. Almost one is required.
 *      - r : (optionnal) all provided coordinates are relative.
 */

task_state_t GCodePipe::line() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //verify that almost one movement coordinate is provided.
    REQUIRE_ONE_ARGUMENTS("XYZE");

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
    CHECK_COORDINATE(x, 'X');
    CHECK_COORDINATE(y, 'Y');
    CHECK_COORDINATE(z, 'Z');
    CHECK_COORDINATE(e, 'E');

    //For safety, un-define the previously created macro.
#undef CHECK_COORDINATE

    //If the movement is absolute, prepare_movement to the destination
    if (CHECK_ARGUMENT('R')) {

        //Mark the movement to be relative.
        state.relative_flag = true;

    }

    //Schedule a line to the specified coordinates
    return MachineController::line_scheduled_0(state);

}


//--------------------------------------------------------Extrusion-----------------------------------------------------

/*
 * enable_steppers : enable or disable steppers.
 *
 *  It takes only one argument_t, -e followed by 0 (disable) or [not zero] enabled
 *
 */

task_state_t GCodePipe::enable_steppers() {


    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //Fail if the enabling argument_t is omitted
    REQUIRE_ALL_ARGUMENTS("E")

    //Extract the enable boolean
    bool enable = (bool) GET_ARG_VALUE('E');

    //Schedule an enable / disable of steppers.
    return MachineController::enable_steppers_scheduled_0(enable);

}



/*
 * set_position : updates the current position.
 *
 *  It takes almost one of the coordinates x, y, z and e.
 *
 */

task_state_t GCodePipe::set_position() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //verify that almost one movement coordinate is provided.
    REQUIRE_ONE_ARGUMENTS("xyze");

    MachineController::offsets_state_t state = MachineController::offsets_state_t();

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


    //Schedule a line to the specified coordinates
    return MachineController::set_current_position_scheduled_0(state);

}


/*
 * set_extrusion : this function modifies extrusion parameters.
 *
 *  It takes the folloging content :
 *      - c : changes the working carriage;
 *      - s : changes the speed for the carriages designed by c (or for the working carriage if c is not provided).
 *
 *  Almost one must be provided.
 *
 */
task_state_t GCodePipe::set_extrusion() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //At least w (working carriage) and s (speed) must be provided
    REQUIRE_ONE_ARGUMENTS("CS");

    MachineController::carriages_state_t new_state = MachineController::carriages_state_t();

    //If the working carriage must be changed
    if (CHECK_ARGUMENT('C')) {

        //Set the flag;
        new_state.working_carriage_flag = true;

        //Set the new working extruder;
        new_state.working_carriage = (uint8_t) GET_ARG_VALUE('w');

    }

    //If the speed must be changed;
    if (CHECK_ARGUMENT('S')) {

        //If we must set the speed for a particular carriage:
        if (CHECK_ARGUMENT('C')) {

            //Enable the speed modif for a carriage;
            new_state.nominative_speed_mod_flag = true;

            //Set the carriage;
            new_state.nominative_carriage =  (uint8_t) GET_ARG_VALUE('C');

            //Set the speed;
            new_state.nominative_speed = GET_ARG_VALUE('S');


        } else {
            //If we must set the speed for the current carriage :

            //Set the flag;
            new_state.working_carriage_speed_flag = true;

            //Set the new working extruder;
            new_state.working_carriage_speed = GET_ARG_VALUE('S');
        }

    }

    //Modify the extrusion state.

    return MachineController::set_carriages_state_scheduled_0(new_state);;

}


/*
 * set_cooling : this function modifies the cooling state.
 *
 *  It takes the following content :
 *      -e : 0 means disable the cooling, other values will enable it.
 *      -p : modifies the cooling power (truncated between 0 and 100).
 *
 */

task_state_t GCodePipe::set_cooling() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //Fail if none of power of enable are provided
    REQUIRE_ONE_ARGUMENTS("EP");

    MachineController::cooling_state_t new_state = MachineController::cooling_state_t();

    //Set the enable state if required
    if (CHECK_ARGUMENT('E')) {


        //Set the flag
        new_state.enabled_flag = true;

        //get the enable boolean
        new_state.enabled = (bool) GET_ARG_VALUE('e');

    }

    //Set the power if required
    if (CHECK_ARGUMENT('P')) {

        //Set the flag
        new_state.enabled_flag = true;

        //Set the new power
        new_state.power = GET_ARG_VALUE('p');

    }

    return MachineController::set_cooling_state_scheduled_0(new_state);;

}


/*
 * set_hotend : this function sets the state of a particular hotend.
 *
 *  It takes the following content :
 *      -h : the hotend to modify, mandatory.
 *      -e : 0 means disable the power on the hotend, other values will enable it.
 *      -t : sets the targetVector temperature of the hotend.
 *
 *      -e or -t must be provided.
 */

task_state_t GCodePipe::set_hotend() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //Fail if a hotend was not specified
    REQUIRE_ALL_ARGUMENTS("H")

    //Fail if neither temperature (t) or enable_state (e) are provided.
    REQUIRE_ONE_ARGUMENTS("TE");
    TemperatureController::hotend_state_t state = TemperatureController::hotend_state_t();

    state.hotend_flag = true;
    state.hotend = (uint8_t) GET_ARG_VALUE('H');

    //If the temperature must be adjusted
    if (CHECK_ARGUMENT('T')) {

        //Set the temperature flag
        state.temperature_flag = true;

        //Get the temperature
        state.temperature = GET_ARG_VALUE('T');

    }

    //If the activity must be changed
    if (CHECK_ARGUMENT('E')) {

        //Set the enable flag
        state.enabled_flag = true;

        //Get the temperature
        state.enabled = (bool) GET_ARG_VALUE('E');

    }

    //Schedule an enable/disable of the hotbed regulation.
    return TemperatureController::set_hotends_state_scheduled_0(state);

}


/*
 * set_hotbed : this function sets the hotbed's state
 *
 *  It takes the following content :
 *      -e : 0 means disable the power on the hotbed, other values will enable it.
 *      -t : sets the targetVector temperature of the hotbed.
 *
 *      -e or -t must be provided.
 */

task_state_t GCodePipe::set_hotbed() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //Fail if neither temperature (t) or enable_state (e) are provided.
    REQUIRE_ONE_ARGUMENTS("TE");
    TemperatureController::hotbed_state_t state = TemperatureController::hotbed_state_t();


    //If the temperature must be adjusted
    if (CHECK_ARGUMENT('T')) {

        //Set the temperature flag
        state.temperature_flag = true;

        //Get the temperature
        state.temperature = GET_ARG_VALUE('T');

    }

    //If the activity must be changed
    if (CHECK_ARGUMENT('E')) {

        //Set the enable flag
        state.enabled_flag = true;

        //Get the temperature
        state.enabled = (bool) GET_ARG_VALUE('E');

    }

    //Schedule an enable/disable of the hotbed regulation.
    return TemperatureController::set_hotbed_state_scheduled_0(state);

}



//---------------------------------------------------------Gets---------------------------------------------------------



task_state_t GCodePipe::get_regulations() {

    return complete;

}


task_state_t GCodePipe::get_temps() {

    return complete;

}



//---------------------------------------------------------Tests--------------------------------------------------------


task_state_t GCodePipe::stepper_test() {

    std_out("EXIT");

    return complete;


}


task_state_t GCodePipe::temp_test() {


    std_out("t0 : " + string(Thermistors::get_temperature_hotend_0(845), 5));
    std_out("t0 : " + string(Thermistors::get_temperature_hotend_0(846), 5));
    std_out("t0 : " + string(Thermistors::get_temperature_hotend_0(847), 5));
    std_out("t0 : " + string(Thermistors::get_temperature_hotend_0(846), 5));
    std_out("t0 : " + string(Thermistors::get_temperature_hotend_0(845), 5));


    return complete;
}

#endif
