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

    static task_state_t carriages_reset();

GENERATE_SCHEDULER(carriages_reset, 1);


    static task_state_t line_to(machine_coords_t coords);

GENERATE_SCHEDULER(line_to, 1, machine_coords_t, coords);


    static task_state_t line_of(machine_coords_t coords);

GENERATE_SCHEDULER(line_of, 1, machine_coords_t, coords);


    //-------------------------------Steppers-------------------------------


    //Enable or disable all steppers
    static task_state_t enable_steppers(bool enable);

GENERATE_SCHEDULER(enable_steppers, 1, bool, enable);


    //-------------------------------Extrusion-------------------------------

public:

    /*
     * A structure for the extrusion management.
     *
     *  This struct will contain:
     *
     *  A readonly (won't affect config if set) variable for the current mode;
     *
     *  flags and data for:
     *      - The working extruder;
     *      - The extruder speeds;
     */

    typedef struct extrusion_state_t {

        //Readonly variable for mode;
        uint8_t mode;

        //Flags
        bool working_extruder_flag = false;
        bool current_speed_flag = false, speed_0_flag = false, speed_1_flag = false, speed_2_flag = false, speed_3_flag = false;

        //Data
        uint8_t working_extruder = 0;
        float current_speed = 0, speed_0 = 0, speed_1 = 0, speed_2 = 0, speed_3 = 0;

    } carriages_state_t;


    /*
     * The Extrusion state modifier : this function can modify the state of the extrusion, and can be scheduled.
     *
     *  It receives a state, vith various flags enabled, and makes the appropriate modifications,
     *      calling simple set functions :
     *
     *      - set_speed;
     *      - set_working_carriage.
     *
     */
    static task_state_t set_extrusion_state(extrusion_state_t data);

    //The Scheduler for the function.
GENERATE_SCHEDULER(set_extrusion_state, 1, extrusion_state_t, data);

    //Get the current extrusion state;
    static const extrusion_state_t get_extrusion_state();


private :

    //The current extrusion state;
    static extrusion_state_t extrusion_state;

    //Mofify the current working extruder.
    static task_state_t set_working_extruder(uint8_t carriage);


    //-------------------------------Cooling-------------------------------

public:

    /*
     * A structure for the cooling state management.
     *
     *  This struct will contain flags and data for:
     *      - enable state (is the cooling enabled)
     *      - the power of the cooling
     */

    typedef struct cooling_state_t {
        //Data flags : set if the data is relevant
        bool enabled_flag = false;
        bool power_flag = false;

        //Data
        bool enabled = false;
        float power = true;

    } cooling_state_t;

    //Set and get hotbed temperatures
    static task_state_t set_cooling_state(cooling_state_t);

GENERATE_SCHEDULER(set_cooling_state, 1, cooling_state_t, state);

    static const cooling_state_t get_cooling_state();


private:

    static cooling_state_t cooling_state;


private:

    static float *const machine_coords;

    static float *const position;

    static task_state_t mode_0(machine_coords_t *coords);

    static void sanity_check(float *hl_coords);

    static void fill_coords(machine_coords_t *coords);

    static void persist_coords(machine_coords_t *coords);

    static void set_reference_carriage(uint8_t id, float x, float y, carriage_data *data);

    static void check_carriage(uint8_t id, float x, float y, carriage_data *data);
};


#endif //TRACER_COORDINATESINTERFACE_H
