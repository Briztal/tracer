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
 * MachineInterface : this class is the interface between the kernel and the
 *
 *  It is also in charge of the current position storage.
 *
 */

#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_STEPPER_ABSTRACTION_H
#define TRACER_STEPPER_ABSTRACTION_H


class MachineInterface {

    //----------------------------------------------------Movements-----------------------------------------------------

public:

    //The basic linear movement, from the current position to the destination position.
    static void linear_movement(float *destination);

    //If the selected kernel is kernel2, then we must register other movements.
    //TODO


    //--------------------------------------------current_stepper_positions---------------------------------------------

public :

    //The function to get the current position
    static void get_current_position(float *const position);

    //The function to update the current position
    static void update_position(const float *const new_position);


private :

    //The current high level position
    static float *current_position;


    //------------------------------------------Coordinate_Systems_Translation------------------------------------------

public:

    //The function to translate a high level position into stepper positions
    static void translate(const float *const hl_coordinates, float *const steppers_coordinates);


    //-------------------------------------------------Speed_Management-------------------------------------------------

public :

    //The function to get the current speed group
    static uint8_t get_speed_group();

    //The function to set the current speed group
    static void set_speed_group(uint8_t speed_group);

    //The function to get the current speed, for the current speed group.
    static float get_speed();

    //The function to set the speed for the provided speed group.
    static void set_speed_for_group(uint8_t speed_group, float new_speed);

    //function to compute the high level movement distance for a particular speed group.
    static float get_movement_distance_for_group(uint8_t speed_group, const float *const distances);


private :

    //The speed groups indices {i_k} where i_(3j+k) (0<=k<3) if the k_th indice of the speed group j. Compiler constant.
    static const int8_t *const speed_groups_indices;

    //the current target speeds, indexed on speed groups
    static float *const speeds;

    //the maximum speeds, indexed on speed groups
    static const float *const max_speeds;

    //The current speed group
    static uint8_t speed_group;


    //-------------------------------------------------Tools_Management-------------------------------------------------

public :

    static void set_energy_density(uint8_t tool_index, float power);

    static sig_t get_tools_data(float *energy_densities);

    static uint8_t set_tools_updating_function(sig_t tools_signature, void (**updating_functions)(float));

    static void stop_tools(sig_t stop_signature);


private :

    static float *tools_energy_densities;

};


#endif //TRACER_STEPPER_ABSTRACTION_H

#endif