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

typedef struct machine_coords_t {

    bool x_enabled = false;
    bool y_enabled = false;
    bool z_enabled = false;
    bool e_enabled = false;

    float x = 0;
    float y = 0;
    float z = 0;
    float e = 0;

} machine_coords_t;


typedef struct carriage_data {
    uint8_t carriage_id;
    float time;
} carriage_data;


class MachineController {



public:

    //-------------------------------Movement-------------------------------

    //r
    static task_state_t carriages_reset();

GENERATE_SCHEDULER(carriages_reset, 1);


    static task_state_t line_to(machine_coords_t coords);

GENERATE_SCHEDULER(line_to, 1, machine_coords_t, coords);


    static task_state_t line_of(machine_coords_t coords);

GENERATE_SCHEDULER(line_of, 1, machine_coords_t, coords);


    //-------------------------------Setup-------------------------------

    //Enable or disable all steppers
    static task_state_t enable_steppers(bool enable);

GENERATE_SCHEDULER(enable_steppers, 1, bool, enable);


    //Set the working extruder
    static task_state_t extruder_set(uint8_t carriage);

GENERATE_SCHEDULER(extruder_set, 1, uint8_t, carriage);


    //Set the speed for a particular extruder
    static task_state_t speed_set(uint8_t carriage, float speed);

GENERATE_SCHEDULER(speed_set, 1, uint8_t, carriage, float, speed);


    //Set the speef for a particular extruder and select it as the working one.
    static task_state_t extruder_speed_set(uint8_t carriage, float speed);

GENERATE_SCHEDULER(extruder_speed_set, 1, uint8_t, carriage, float, speed);



    //-------------------------------Cooling-------------------------------


    //Set and get hotbed temperatures
    static task_state_t set_cooling_power(float power);

GENERATE_SCHEDULER(set_cooling_power, 1, float, power);


    //Not scheduled : get the CURRENT cooling power
    static float get_cooling_power();


    //enable - disable hotbed
    static task_state_t enable_cooling(bool enable);

GENERATE_SCHEDULER(enable_cooling, 1, bool, state);


    //Not scheduled : is the cooling CURRENTLY enabled
    static bool is_cooling_enabled();


private:

    static float *const machine_coords;

    static uint8_t mode;

    static uint8_t carriage_id;

    static float *const position;

    static float cooling_power;

    static task_state_t mode_0(machine_coords_t *coords);

    static void sanity_check(float *hl_coords);

    static void fill_coords(machine_coords_t *coords);

    static void persist_coords(machine_coords_t *coords);

    static void set_reference_carriage(uint8_t id, float x, float y, carriage_data *data);

    static void check_carriage(uint8_t id, float x, float y, carriage_data *data);
};


#endif //TRACER_COORDINATESINTERFACE_H
