/*
  LCDInterfaceCommands.cpp - Part of TRACER

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

#ifdef ENABLE_LCD_INTERFACE

#include "LCDInterface.h"
#include "LCDInterfaceCommands.h"
#include <StepperControl/MachineInterface.h>
#include <Actions/ContinuousActions.h>
#include <Core/EEPROMStorage.h>
#include <interface.h>

//---------------------------------------------------Custom functions---------------------------------------------------

bool LCDInterfaceCommands::action(char *, uint8_t) {

    TI::echo("action");
    return true;

}


bool LCDInterfaceCommands::home(char *, uint8_t) {

    TI::echo("home");
    return false;

}


bool LCDInterfaceCommands::line(char *, uint8_t) {

    TI::echo("line");
    return true;

}


bool LCDInterfaceCommands::speed(char *, uint8_t) {

    TI::echo("speed");
    return false;

}


#endif

