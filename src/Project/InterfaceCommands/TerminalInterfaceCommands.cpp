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
#include <EEPROMStorage/EEPROMStorage.h>
#include <interface.h>
#include <StepperControl/Machine.h>
#include <TaskScheduler/TaskScheduler.h>
#include <EEPROMStorage/EEPROMInterface.h>
#include <DataStructures/StringParser.h>
#include <StepperControl/StepperController.h>
#include <Sensors/Thermistors/Thermistors.h>


task_state_t TerminalInterfaceCommands::action(uint8_t args_index) {

    Machine::set_speed_for_carriage(0, 150);
    Machine::set_carriage(0);
    Machine::line_to(50, 50, 0);
    Machine::line_to(100, 100, 0);

    Machine::zero_carriages();

    Machine::set_speed_for_carriage(1, 150);
    Machine::set_carriage(1);
    Machine::line_to(50, 50, 0);
    Machine::line_to(100, 100, 0);

    Machine::zero_carriages();

    Machine::set_speed_for_carriage(2, 150);
    Machine::set_carriage(2);
    Machine::line_to(50, 50, 0);
    Machine::line_to(100, 100, 0);

    Machine::zero_carriages();

    Machine::set_speed_for_carriage(3, 150);
    Machine::set_carriage(3);
    Machine::line_to(50, 50, 0);
    Machine::set_carriage(0);
    Machine::line_to(100, 100, 0);

    Machine::zero_carriages();

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


task_state_t TerminalInterfaceCommands::move_home(uint8_t args_index) {

    CI::echo("ENDSTOPS NOT SUPPORTED FOR INSTANCE");

    return complete;

}


task_state_t TerminalInterfaceCommands::move_zero(uint8_t args_index) {

    CI::echo("Reseting carriages");

    Machine::zero_carriages();

    return complete;


}

task_state_t TerminalInterfaceCommands::set_carriage(uint8_t args_index) {

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

    task_state_t state = Machine::set_carriage_and_speed(carriage_id, speed);



    if (state == complete) {

        CI::echo("Carriage modified. Moving carriages to zero...");

        Machine::zero_carriages();
    }

    return state;

}


task_state_t TerminalInterfaceCommands::line_to(uint8_t args_index) {

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

    //Draw a line to the specified coordinates with the specified carriage_id
    return Machine::line_to(x, y, z);

}


task_state_t TerminalInterfaceCommands::line_of(uint8_t args_index) {

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

    //Draw a line to the specified coordinates with the specified carriage_id
    return Machine::line_of(x, y, z);

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
    Machine::set_speed_for_carriage(carriage_id, speed);

    return complete;

}


task_state_t TerminalInterfaceCommands::stepper_test(uint8_t args_index) {



    while(true) {
        //digital_write(15, LOW);
        //digital_write(14, LOW);
        digital_write(13, LOW);
    }

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

