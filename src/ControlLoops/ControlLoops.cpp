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

#include <interface.h>

#include "ControlLoops.h"
#include <Project/TemperatureController.h>

/*
 * initialisation_function_temperature : The function called when the regulation is enabled.
 *
 * It calls the inline function TemperatureController::regulation_init().

 *
 */

void ControlLoops::initialisation_function_temperature() {

    TemperatureController::regulation_init();

}


/*
 * loop_function_temperature : The function called when the interrupt occurs.
 *
 * It calls the inline function TemperatureController::temperature_regulation().
 *
 * Do not forget to disable the interrupt that
 *
 */

void ControlLoops::loop_function_temperature() {

    disable_loop_interrupt_0();

    TemperatureController::temperature_regulation();

    enable_loop_interrupt_0();

}


/*
 * finalisation_function_temperature : The function called when the regulation is disabled.
 *
 */

void ControlLoops::finalisation_function_temperature() {

    TemperatureController::regulation_finalisation();

}



