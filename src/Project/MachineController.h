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

class MachineController {



    //-------------------------------Homing movement-------------------------------

public:

    static task_state_t home();

    //A scheduler for the cooling homing functione
GENERATE_SCHEDULER(home, 0);

private:

    typedef struct carriage_data {
        uint8_t carriage_id;
        float time;
    } carriage_data;


    //Set the reference carriage, in the homing preparation
    static void set_reference_carriage(uint8_t id, float x, float y, carriage_data *data);

    //Check if a carriage do not exceeds its limit, during the homing preparation.
    static void check_carriage(uint8_t id, float x, float y, carriage_data *data);


    //-------------------------------Line movement-------------------------------

public:

    typedef struct machine_coords_t {

        //Flag for relative movement, default to false -> movement absolute by default.
        bool relative_flag = false;

        //Flags for coordinates
        bool x_flag = false, y_flag = false, z_flag = false, e_flag = false;

        //Flags for current carriage and speed.
        bool extruder_flag = false, speed_flag = false;

        //Coordinates
        float x = 0, y = 0, z = 0, e = 0;

        //Current carriage
        uint8_t extruder = 0;

        //Speed
        float speed = 0;

    } movement_state_t;


    static task_state_t line(movement_state_t movement_state);

    //A scheduler for the line function.
GENERATE_SCHEDULER(line, 0, machine_coords_t, coords);


private:

    //The current absolute high level machine coordinates : [x, y, z, e].
    static float *const current_position;

    //The current axis coordinates.
    static float *const current_axis_positions;

    //Replace coordinates with flags reset with current ones.
    static void replace_coords(movement_state_t *coords);

    //Save control coords
    static void persist_coords(movement_state_t *coords);

    //movement in mode zero, all carriages accessible.
    static task_state_t move_mode_0(movement_state_t *coords);

    //Sanity check, verify that carriage do not collide, or don't go in negative positions.
    static void sanity_check(float *hl_coords);



    //-------------------------------Steppers-------------------------------

public:

    //Enable or disable all steppers
    static task_state_t enable_steppers(bool enable);

    //A scheduler for the stepper manager
GENERATE_SCHEDULER(enable_steppers, 0, bool, enable);


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
        uint8_t mode = 0;

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
GENERATE_SCHEDULER(set_extrusion_state, 0, extrusion_state_t, data);

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

    //A scheduler for the cooling manager
GENERATE_SCHEDULER(set_cooling_state, 0, cooling_state_t, state);


    static const cooling_state_t get_cooling_state();


private:

    //The cooling state, that contains all current cooling parameters.
    static cooling_state_t cooling_state;

};


#endif //TRACER_COORDINATESINTERFACE_H
