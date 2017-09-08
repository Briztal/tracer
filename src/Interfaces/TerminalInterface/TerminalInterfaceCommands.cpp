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

#include "TerminalInterface.h"
#include "TerminalInterfaceCommands.h"
#include <StepperControl/MachineInterface.h>
#include <Actions/ContinuousActions.h>
#include <Core/EEPROMStorage.h>
#include <interface.h>


//---------------------------------------------------Custom functions---------------------------------------------------

bool TerminalInterfaceCommands::action(char *, uint8_t) {

    UI::echo("action");
    return true;

}

bool TerminalInterfaceCommands::home(char *, uint8_t) {

    UI::echo("home");
    return false;

}


bool TerminalInterfaceCommands::line(char *, uint8_t) {

    UI::echo("line");
    return true;


}

bool TerminalInterfaceCommands::speed(char *, uint8_t) {

    UI::echo("speed");
    return false;

}


#endif

