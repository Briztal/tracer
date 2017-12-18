/*
  StepperAbstraction.h - Part of TRACER

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


/*
 * StepperController : this class is the interface between the kernel and the
 *
 *  It is also in charge of the current position storage.
 *
 */

#include <config.h>
#include <TaskScheduler/task_state_t.h>
#include "stdint.h"
#include <Config/stepper_control_config.h>

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_STEPPER_ABSTRACTION_H
#define TRACER_STEPPER_ABSTRACTION_H


class StepperController {

    //---------------------------------------------------- Init -----------------------------------------------------

public:

    //Initialise the steppers GPIO;
    static void initialise_hardware();

    //Initialise all modules;
    static void initialise_data();


private:

    //Initialise local data;
    static void _initialise_data();


    //----------------------------------------------------Movements-----------------------------------------------------

public:

    //The basic linear movement, from the current position to the destination position;
    static task_state_t linear_movement(float *destination);

    //If the selected kernel is kernel2, then we must register other movements;
    //TODO


    //--------------------------------------------current_steppers_positions---------------------------------------------

public :

    //The function to get the current position;
    static void get_current_position(float *const position);

    //The function to update the current position;
    static void update_position(const float *const new_position);


private :

    //The current high level position;
    static float *current_position;


    //------------------------------------------Coordinate_Systems_Translation------------------------------------------

public:

    //The function to translate a high level position into stepper positions;
    static void translate(const float *const hl_coordinates, float *const steppers_coordinates);

    //The function to get stepper positions for a particular trajectory at a particular point;
    static void get_stepper_positions_for(void (*trajectory_function)(float, float*), float point, float *steppers_positions);


    //-------------------------------------------------Speed_Management-------------------------------------------------

public :

    //The function to get the current speed group;
    static uint8_t get_speed_group();

    //The function to set the current speed group;
    static void set_speed_group(uint8_t speed_group);

    //The function to get the current regulation_speed, for the current regulation_speed group;
    static float get_speed();

    //The function to get the current regulation_speed, for the speed group provided;
    static float get_speed(uint8_t speed_group);

    //The function to set the regulation_speed for the provided regulation_speed group;
    static task_state_t set_speed_for_group(uint8_t speed_group, float new_speed);

    //function to compute the high level movement distance for a particular speed group;
    static float get_movement_distance_for_group(uint8_t speed_group, const float *const distances);


private :

    //The regulation_speed groups indices {i_k} where i_(3j+k) (0<=k<3) if the k_th indice of the regulation_speed group j. Compiler constant.
    static const int8_t *const speed_groups_indices;

    //the current target speeds, indexed on speed groups
    static float *const speeds;

    //the maximum speeds, indexed on speed groups
    static float *const max_speeds;

    //The current speed group
    static uint8_t speed_group;


    //-------------------------------------------------Tools_Management-------------------------------------------------

public :

    //Set the energy density for a particular tool;
    static void set_energy_density(uint8_t tool_index, float power);

    //Get the signature of tools enabled, and appropriate energy densities
    static sig_t get_tools_data(float *energy_densities);

    //Update functions that will be used to control power in real time;
    static uint8_t set_tools_updating_function(sig_t tools_signature, void (**updating_functions)(float));

    //Stop all tools;
    static void stop_tools(sig_t stop_signature);


private :

    //Energy densities;
    static float *tools_energy_densities;

};


#endif //TRACER_STEPPER_ABSTRACTION_H

#endif