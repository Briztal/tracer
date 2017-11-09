/*
  ControlLoops.cpp - Part of TRACER

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

#include "PID.h"
#include "ControlLoops.h"

#include <interface.h>

void ControlLoops::initialisation_function_temperature() {
    PID::reset_hotend_0();
    PID::reset_hotend_1();
    PID::reset_hotend_2();
    PID::reset_hotend_3();
    PID::reset_hotbed();
}


void ControlLoops::loop_function_temperature() {

    disable_loop_interrupt_0();

    float error = 0.1;

    float p = PID::compute_hotend_0(error);

    CI::echo("hotend "+str(p));

    enable_loop_interrupt_0();

}

void ControlLoops::finalisation_function_temperature() {}



