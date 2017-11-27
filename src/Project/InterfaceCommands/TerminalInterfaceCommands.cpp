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


    CI::echo("EXIT");

    return complete;

}


//--------------------------------------------------------EEPROM--------------------------------------------------------

task_state_t TerminalInterfaceCommands::eeprom_write(uint8_t args_index) {

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    if (GET_NB_WORDS(args) < 2) {
        return invalid_arguments;
    }
    
    //Extract the value to write
    GET_NEXT_WORD(args);
    float f = str_to_float(word_buffer);
    
    EEPROMInterface::write_data_by_string(args, f);

    CI::echo("EXIT " + String(f));

    return complete;

}


task_state_t TerminalInterfaceCommands::eeprom_read(uint8_t args_index) {

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    if (GET_NB_WORDS(args) < 1) {
        return invalid_arguments;;
    }

    float f = 0;

    if (EEPROMInterface::read_data_by_string(args, &f)) {
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

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    //Fail if there are not exactly four arguments
    if (GET_NB_WORDS(args) != 4) {
        return invalid_arguments;
    }

    machine_coords_t t = machine_coords_t();
    t.x_enabled = t.y_enabled = t.z_enabled = t.e_enabled = true;


    //Extract x
    GET_NEXT_WORD(args);
    t.x = str_to_float(word_buffer);

    //Extract y
    GET_NEXT_WORD(args);
    t.y = str_to_float(word_buffer);

    //Extract z
    GET_NEXT_WORD(args);
    t.z = str_to_float(word_buffer);

    //Extract e
    GET_NEXT_WORD(args);
    t.e = str_to_float(word_buffer);

    //Schedule a line to the specified coordinates
    return MachineController::line_to_scheduled(t);

}


task_state_t TerminalInterfaceCommands::line_of(uint8_t args_index) {

    //This task requires the schedule of one type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    //Fail if there are not exactly four arguments
    if (GET_NB_WORDS(args) != 4) {
        return invalid_arguments;
    }

    machine_coords_t t = machine_coords_t();
    t.x_enabled = t.y_enabled = t.z_enabled = t.e_enabled = true;

    //Extract x
    GET_NEXT_WORD(args);
    t.x = str_to_float(word_buffer);

    //Extract y
    GET_NEXT_WORD(args);
    t.y = str_to_float(word_buffer);

    //Extract z
    GET_NEXT_WORD(args);
    t.z = str_to_float(word_buffer);

    //Extract e
    GET_NEXT_WORD(args);
    t.e = str_to_float(word_buffer);

    //Schedule a line to the specified coordinates with the specified carriage_id
    return MachineController::line_of_scheduled(t);

}

//---------------------------------------------------------Setup--------------------------------------------------------

task_state_t TerminalInterfaceCommands::set_speed(uint8_t args_index) {

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    //Fail if there are not exactly two arguments
    if (GET_NB_WORDS(args) != 2) {
        return invalid_arguments;
    }

    //Get the carriage_id
    GET_NEXT_WORD(args);
    uint8_t carriage_id = (uint8_t) str_to_float(word_buffer);

    //Get the speed
    GET_NEXT_WORD(args);
    float speed = str_to_float(word_buffer);

    CI::echo("Setting the carriage_id "+String(carriage_id)+" to "+String(speed)+".");
    MachineController::speed_set_scheduled(carriage_id, speed);

    return complete;

}


task_state_t TerminalInterfaceCommands::set_carriage(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(2);

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    //Fail if there are not exactly two arguments
    if (GET_NB_WORDS(args) != 2) {
        return invalid_arguments;
    }

    //Extract carriage_id id
    GET_NEXT_WORD(args);
    uint8_t carriage_id = (uint8_t) str_to_float(word_buffer);

    //Extract carriage_id id
    GET_NEXT_WORD(args);
    float speed = str_to_float(word_buffer);


    task_state_t state = MachineController::extruder_speed_set_scheduled(carriage_id, speed);

    if (state == complete) {

        CI::echo("Carriage modified. Moving carriages to zero...");

        MachineController::carriages_reset_scheduled();
    }

    return state;

}



task_state_t TerminalInterfaceCommands::enable_steppers(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    //Fail if there are not exactly one argument
    if (GET_NB_WORDS(args) != 1) {
        return invalid_arguments;
    }

    //Extract the enable boolean
    GET_NEXT_WORD(args);
    bool enable = (bool) str_to_float(word_buffer);

    //Schedule an enable / disable of steppers.
    return MachineController::enable_steppers_scheduled(enable);

}



task_state_t TerminalInterfaceCommands::enable_cooling(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    //Fail if there are not exactly one argument
    if (GET_NB_WORDS(args) != 1) {
        return invalid_arguments;
    }

    //Extract the enable boolean
    GET_NEXT_WORD(args);
    bool enable = (bool) str_to_float(word_buffer);

    //Schedule an enable / disable of cooling.
    return MachineController::enable_cooling_scheduled(enable);

}


task_state_t TerminalInterfaceCommands::set_cooling_power(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    //Fail if there are not exactly one argument
    if (GET_NB_WORDS(args) != 1) {
        return invalid_arguments;
    }

    //Extract the power
    GET_NEXT_WORD(args);
    float power = str_to_float(word_buffer);

    //Schedule a modification of the cooling power
    return MachineController::set_cooling_power_scheduled(power);

}




task_state_t TerminalInterfaceCommands::enable_bed(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    //Fail if there are not exactly one argument
    if (GET_NB_WORDS(args) != 1) {
        return invalid_arguments;
    }

    //Extract the enable boolean
    GET_NEXT_WORD(args);
    bool enable = (bool) str_to_float(word_buffer);

    //Schedule an enable/disable of the hotbed regulation.
    return TemperatureController::enable_hotbed_regulation_scheduled(enable);

}


task_state_t TerminalInterfaceCommands::set_bed_temp(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    //Fail if there are not exactly one argument
    if (GET_NB_WORDS(args) != 1) {
        return invalid_arguments;
    }

    //Extract the temperature
    GET_NEXT_WORD(args);
    float temp = str_to_float(word_buffer);

    //Schedule a modification of the hotbed power
    return TemperatureController::set_hotbed_temperature_scheduled(temp);


}



task_state_t TerminalInterfaceCommands::enable_hotend(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    //Fail if there are not exactly two arguments
    if (GET_NB_WORDS(args) != 2) {
        return invalid_arguments;
    }

    //Extract the enable boolean
    GET_NEXT_WORD(args);
    uint8_t hotend = (uint8_t) str_to_float(word_buffer);

    //Extract the enable boolean
    GET_NEXT_WORD(args);
    bool enable = (bool) str_to_float(word_buffer);

    //Schedule an enable/disable of the hotbed regulation.
    return TemperatureController::enable_hotend_regulation_scheduled(hotend, enable);

}



task_state_t TerminalInterfaceCommands::set_hotend_temp(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare and define args, and give them the correct value
    GET_ARGS(args_index, args);

    //Fail if there are not exactly two arguments
    if (GET_NB_WORDS(args) != 2) {
        return invalid_arguments;
    }

    //Extract the enable boolean
    GET_NEXT_WORD(args);
    uint8_t hotend = (uint8_t) str_to_float(word_buffer);

    //Extract the enable boolean
    GET_NEXT_WORD(args);
    float temperature = str_to_float(word_buffer);

    //Schedule an enable/disable of the hotbed regulation.
    return TemperatureController::set_hotend_temperature_scheduled(hotend, temperature);

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

