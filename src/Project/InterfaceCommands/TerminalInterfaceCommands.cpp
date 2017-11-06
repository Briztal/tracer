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
#include <StepperControl/Machine.h>
#include <TaskScheduler/TaskScheduler.h>
#include <EEPROM/EEPROMInterface.h>
#include <DataStructures/StringParser.h>
#include <StepperControl/StepperController.h>
#include <Sensors/Thermistors/Thermistors.h>


task_state_t TerminalInterfaceCommands::action(uint8_t args_index) {

    Machine::speed_set_scheduled(0, 150);
    Machine::carriage_set_scheduled(0);
    Machine::line_to_scheduled(50, 50, 0);
    Machine::line_to_scheduled(100, 100, 0);

    Machine::carriages_reset_scheduled();

    Machine::speed_set_scheduled(1, 150);
    Machine::carriage_set_scheduled(1);
    Machine::line_to_scheduled(50, 50, 0);
    Machine::line_to_scheduled(100, 100, 0);

    Machine::carriages_reset_scheduled();

    Machine::speed_set_scheduled(2, 150);
    Machine::carriage_set_scheduled(2);
    Machine::line_to_scheduled(50, 50, 0);
    Machine::line_to_scheduled(100, 100, 0);

    Machine::carriages_reset_scheduled();

    Machine::speed_set_scheduled(3, 150);
    Machine::carriage_set_scheduled(3);
    Machine::line_to_scheduled(50, 50, 0);
    Machine::carriage_set_scheduled(0);
    Machine::line_to_scheduled(100, 100, 0);

    Machine::carriages_reset_scheduled();

    float position[NB_AXIS]{0};
    position[0] = 500;
    position[1] = 500;
    position[2] = 500;
    position[3] = 500;

    MachineInterface::set_speed_group(0);
    MachineInterface::set_speed_for_group(0, 200);

    MachineInterface::linear_movement(position);

    CI::echo("EXIT");

    return complete;

}


//--------------------------------------------------------EEPROM--------------------------------------------------------

task_state_t TerminalInterfaceCommands::eeprom_write(uint8_t args_index) {

    //Declare args, size, and give them the correct value
    GET_ARGS(args_index, args, size);

    if (GET_NB_WORDS(args, size) < 2) {
        return invalid_arguments;
    }

    StringParser::get_next_word(&args, &size);

    float f = str_to_float(WORD);

    EEPROMInterface::write_data_by_string(args, size, f);

    CI::echo("EXIT " + String(f));

    return complete;

}


task_state_t TerminalInterfaceCommands::eeprom_read(uint8_t args_index) {

    //Declare args, size, and give them the correct value
    GET_ARGS(args_index, args, size);

    if (GET_NB_WORDS(args, size) < 1) {
        return invalid_arguments;;
    }

    float f = 0;

    if (EEPROMInterface::read_data_by_string(args, size, &f)) {
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
    Machine::carriages_reset_scheduled();

    return complete;


}


task_state_t TerminalInterfaceCommands::line_to(uint8_t args_index) {

    //This task requires the schedule of one type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare args, size, and give them the correct value
    GET_ARGS(args_index, args, size);

    if (GET_NB_WORDS(args, size) != 3) {
        return invalid_arguments;
    }

    //Extract x
    GET_NEXT_WORD(args, size);
    float x = str_to_float(WORD);

    //Extract y
    GET_NEXT_WORD(args, size);
    float y = str_to_float(WORD);

    //Extract z
    GET_NEXT_WORD(args, size);
    float z = str_to_float(WORD);

    //Schedule a line to the specified coordinates with the specified carriage_id
    return Machine::line_to_scheduled(x, y, z);

}


task_state_t TerminalInterfaceCommands::line_of(uint8_t args_index) {

    //This task requires the schedule of one type-0 task
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare args, size, and give them the correct value
    GET_ARGS(args_index, args, size);

    if (GET_NB_WORDS(args, size) != 3) {
        return invalid_arguments;
    }

    //Extract x
    GET_NEXT_WORD(args, size);
    float x = str_to_float(WORD);

    //Extract y
    GET_NEXT_WORD(args, size);
    float y = str_to_float(WORD);

    //Extract z
    GET_NEXT_WORD(args, size);
    float z = str_to_float(WORD);

    //Schedule a line to the specified coordinates with the specified carriage_id
    return Machine::line_of_scheduled(x, y, z);

}

//---------------------------------------------------------Setup--------------------------------------------------------

task_state_t TerminalInterfaceCommands::set_speed(uint8_t args_index) {

    //Declare args, size, and give them the correct value
    GET_ARGS(args_index, args, size);

    if (GET_NB_WORDS(args, size) != 2) {
        return invalid_arguments;
    }

    //Get the carriage_id
    GET_NEXT_WORD(args, size);
    uint8_t carriage_id = (uint8_t) str_to_float(WORD);

    //Get the speed
    GET_NEXT_WORD(args, size);
    float speed = str_to_float(WORD);

    CI::echo("Setting the carriage_id "+String(carriage_id)+" to "+String(speed)+".");
    Machine::speed_set_scheduled(carriage_id, speed);

    return complete;

}


task_state_t TerminalInterfaceCommands::set_carriage(uint8_t args_index) {

    //This task requires the schedule of two type-0 task
    FAIL_IF_CANT_SCHEDULE(2);

    //Declare args, size, and give them the correct value
    GET_ARGS(args_index, args, size);

    if (GET_NB_WORDS(args, size) != 2) {
        return invalid_arguments;
    }

    //Extract carriage_id id
    GET_NEXT_WORD(args, size);
    uint8_t carriage_id = (uint8_t) str_to_float(WORD);

    //Extract carriage_id id
    GET_NEXT_WORD(args, size);
    float speed = str_to_float(WORD);


    task_state_t state = Machine::carriage_speed_set_scheduled(carriage_id, speed);

    if (state == complete) {

        CI::echo("Carriage modified. Moving carriages to zero...");

        Machine::carriages_reset_scheduled();
    }

    return state;

}

//---------------------------------------------------------Tests--------------------------------------------------------


task_state_t TerminalInterfaceCommands::stepper_test(uint8_t args_index) {



}




task_state_t TerminalInterfaceCommands::temp_test(uint8_t args_index) {

    CI::echo("t0 : "+String(Thermistors::get_temperature_0(), 5));
    CI::echo("t1 : "+String(Thermistors::get_temperature_1(), 5));
    CI::echo("t2 : "+String(Thermistors::get_temperature_2(), 5));
    CI::echo("t3 : "+String(Thermistors::get_temperature_3(), 5));
    CI::echo("t4 : "+String(Thermistors::get_temperature_4(), 5));

    return complete;
}


task_state_t TerminalInterfaceCommands::heater_test(uint8_t args_index) {

    CI::echo("SUUS");
    ContinuousActions::set_power0(100);
    ContinuousActions::set_power1(100);
    ContinuousActions::set_power2(100);
    ContinuousActions::set_power3(100);
    ContinuousActions::set_power4(100);
    ContinuousActions::set_power5(100);

    CI::echo("ENBALE");

    delay(100000);

    ContinuousActions::set_power0(0);
    ContinuousActions::set_power1(0);
    ContinuousActions::set_power2(0);
    ContinuousActions::set_power3(0);
    ContinuousActions::set_power4(0);
    ContinuousActions::set_power5(0);

    CI::echo("OFF");


    return complete;

}



#endif

