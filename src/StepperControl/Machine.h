/*
  Machine.cpp - Part of TRACER

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


#ifndef TRACER_COORDINATESINTERFACE_H
#define TRACER_COORDINATESINTERFACE_H


#include <cstdint>
#include <TaskScheduler/task_state_t.h>
#include <TaskScheduler/TaskScheduler.h>


class Machine {


    //-------------------------------------------------Power------------------------------------------------------------

public:

    static void disable_stepper_power();

    //Task interfaces :

public:

    //-------------------------------Move-------------------------------

    static task_state_t carriages_reset();

GENERATE_SCHEDULER(carriages_reset, 1);


    static task_state_t line_to(float x, float y, float z);

GENERATE_SCHEDULER(line_to, 1, float, x, float, y, float, z);


    static task_state_t line_of(float x, float y, float z);

GENERATE_SCHEDULER(line_of, 1, float, x, float, y, float, z);


    //-------------------------------Setup-------------------------------

    static task_state_t extruder_set(uint8_t carriage);

GENERATE_SCHEDULER(extruder_set, 1, uint8_t, carriage);


    static task_state_t speed_set(uint8_t carriage, float speed);

GENERATE_SCHEDULER(speed_set, 1, uint8_t, carriage, float, speed);


    static task_state_t extruder_speed_set(uint8_t carriage, float speed);

GENERATE_SCHEDULER(extruder_speed_set, 1, uint8_t, carriage, float, speed);


    //-------------------------------Hotends-------------------------------


    //Set and get hotends temperatures

    static task_state_t set_hotend_temperature(uint8_t hotend, float temp);

GENERATE_SCHEDULER(set_hotend_temperature, 1, uint8_t, hotend, float, temp);


    static task_state_t get_hotend_temperature(uint8_t hotend);

GENERATE_SCHEDULER(get_hotend_temperature, 1, uint8_t, hotend);

    //enable - disable hotends

    static task_state_t set_hotend_state(uint8_t hotend, bool state);

GENERATE_SCHEDULER(set_hotend_state, 1, uint8_t, hotend, bool, state);


    static task_state_t get_hotend_state(uint8_t hotend);

GENERATE_SCHEDULER(get_hotend_state, 1, uint8_t, hotend);


    //-------------------------------Hotbed-------------------------------

    //Set and get hotbed temperatures

    static task_state_t set_hotbed_temperature(float temp);

GENERATE_SCHEDULER(set_hotbed_temperature, 1, float, temp);


    static task_state_t get_hotbed_temperature();

GENERATE_SCHEDULER(get_hotbed_temperature, 1);


    //enable - disable hotbed

    static task_state_t set_hotbed_state(bool state);

GENERATE_SCHEDULER(set_hotbed_state, 1, bool, state);


    static task_state_t get_hotbed_state();

GENERATE_SCHEDULER(get_hotbed_state, 1);

    //-------------------------------Cooling-------------------------------


    //Set and get hotbed temperatures

    static task_state_t set_cooling_power(float temp);

GENERATE_SCHEDULER(set_cooling_power, 1, float, temp);


    static task_state_t get_cooling_power();

GENERATE_SCHEDULER(get_cooling_power, 1);


    //enable - disable hotbed

    static task_state_t set_cooling_state(bool state);

GENERATE_SCHEDULER(set_cooling_state, 1, bool, state);


    static task_state_t get_cooling_state();

GENERATE_SCHEDULER(get_cooling_state, 1);



private:

    static float *const machine_coords;

    static uint8_t mode;

    static uint8_t carriage_id;

    static float *const position;

    static float cooling_power;

    static task_state_t mode_0(float x, float y, float z);

    static void sanity_check(float *hl_coords);

};


#endif //TRACER_COORDINATESINTERFACE_H
