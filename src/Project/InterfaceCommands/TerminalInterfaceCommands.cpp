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


uint8_t TerminalInterfaceCommands::action(uint8_t args_index) {

    Machine::line_to(0, 50, 50, 0, 150);
    Machine::line_to(0, 100, 100, 0, 150);

    Machine::home_carriages(100);

    Machine::line_to(1, 50, 50, 0, 150);
    Machine::line_to(1, 100, 100, 0, 150);

    Machine::home_carriages(100);

    Machine::line_to(2, 50, 50, 0, 150);
    Machine::line_to(2, 100, 100, 0, 150);

    Machine::home_carriages(100);

    Machine::line_to(3, 50, 50, 0, 150);
    Machine::line_to(3, 100, 100, 0, 150);

    Machine::home_carriages(100);

    float position[NB_AXIS] {0};
    position[0] = 500;
    position[1] = 500;
    position[2] = 500;
    position[3] = 500;

    MachineInterface::set_speed_group(0);
    MachineInterface::set_speed_for_group(0, 200);

    MachineInterface::linear_movement(position);

    CI::echo("EXIT");

    RETURN_NO_ERROR

}

uint8_t TerminalInterfaceCommands::stress(uint8_t args_index) {

    char *args;
    uint8_t size;

    GET_ARGS(args_index, args, size);

    CI::echo("NB OF ARGS : "+String(size));

    CI::echo("EXIT");

    RETURN_NO_ERROR;

}


uint8_t TerminalInterfaceCommands::home(uint8_t args_index) {

    UI::echo("home");
    RETURN_ARGUMENT_ERROR;

}


uint8_t TerminalInterfaceCommands::line(uint8_t args_index) {

    UI::echo("line");
    RETURN_NO_ERROR


}

uint8_t TerminalInterfaceCommands::speed(uint8_t args_index) {

    UI::echo("speed");
    RETURN_ARGUMENT_ERROR;

}


#endif

