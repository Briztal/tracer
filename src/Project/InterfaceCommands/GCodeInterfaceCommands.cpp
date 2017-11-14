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


#include "config.h"

#ifdef ENABLE_GCODE_INTERFACE

#include "GCodeInterfaceCommands.h"
#include <Interfaces/GCodeInterface/GCodeInterface.h>

task_state_t GCodeInterfaceCommands::g0(void *) {
    GI::echo("SUUSg0");
    return complete;
}

task_state_t GCodeInterfaceCommands::g1(void *) {
    GI::echo("SUUSg1");
    return complete;
}

task_state_t GCodeInterfaceCommands::g10(void *) {
    GI::echo("SUUSg10");
    return complete;
}

task_state_t GCodeInterfaceCommands::g5d0(void *) {
    GI::echo("SUUSg5d0");
    return complete;
}

task_state_t GCodeInterfaceCommands::m1(void *) {
    GI::echo("SUUSm1");
    return complete;
}

task_state_t GCodeInterfaceCommands::m2(void *) {
    GI::echo("SUUSm2");
    return complete;
}

task_state_t GCodeInterfaceCommands::m225(void *) {
    GI::echo("SUUSm225");
    return complete;
}

#endif
