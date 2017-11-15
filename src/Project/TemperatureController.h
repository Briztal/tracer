/*
  TempControl.h - Part of TRACER

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


#ifndef TRACER_CLOSEDLOOPS_H
#define TRACER_CLOSEDLOOPS_H


#include <Sensors/Thermistors/Thermistors.h>
#include <ControlLoops/PID.h>
#include <Actions/ContinuousActions.h>
#include "TemperatureController.h"
#include <interface.h>
#include <TaskScheduler/TaskScheduler.h>

class TemperatureController {


public:

    //---------------------------Interrupt regulation functions---------------------------

    //The function to initialise the temperature regulation routine
    static void regulation_init();

    //Temperature regulation routine.
    static void temperature_regulation();

    //The function to finalise the temperature regulation routine
    static void regulation_finalisation();


    //---------------------------State selection functions---------------------------

    //Hotends
    static task_state_t enable_hotend_regulation(uint8_t hotend, bool enable);

GENERATE_SCHEDULER(enable_hotend_regulation, 2, uint8_t, hotend, bool, enable);


    static task_state_t set_hotend_target(uint8_t hotend, float target);

GENERATE_SCHEDULER(set_hotend_target, 2, uint8_t, hotend, float, target);


    //Hotbed
    static task_state_t enable_hotbed_regulation(bool enable);

GENERATE_SCHEDULER(enable_hotbed_regulation, 2, bool, enable);


    static task_state_t set_hotbed_temperature(float target);

GENERATE_SCHEDULER(set_hotbed_temperature, 1, float, target);


private:

    //The temperature targets
    static float *const temp_targets;

    //The regulation flags : i-th is set if the i-th thermistor's regulation is enabled.
    static bool *const temp_regulation_enabled;


};


#endif //TRACER_CLOSEDLOOPS_H
