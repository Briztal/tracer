/*
  KinematicsCore1.h - Part of TRACER

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

#include <config.h>

#if defined(ENABLE_STEPPER_CONTROL) && (CORE_VERSION == 1)


#ifndef TRACER_KERNEL1_H
#define TRACER_KERNEL1_H

#include <stdint.h>
#include <StepperControl/_kinematics_data.h>

class KinematicsCore1 {

public :


    //------------------------------------------- Initialisation --------------------------------------------

public:

    static void initialise_data();


    //------------------------------------------------ start ------------------------------------------------

public:

    static void start_tracing_procedure();


    //-------------------------------------------Current high level position--------------------------------------------

private :

    //The current position in the high level system.
    static float *const current_hl_position;


    //---------------------------------------------Current movement update----------------------------------------------

public:

    static void initialise_kinetics_data(k1_movement_data *movement_data);

    static void load_pre_process_kinetics_data(k1_movement_data *movement_data);

    static void compute_jerk_offsets(float speed, k1_movement_data *previous_movement);


    //-------------------------------------------real_time_environment update-------------------------------------------

public :

    static void load_real_time_kinetics_data(const k1_movement_data *movement_data);

    static void load_real_time_jerk_data(const k1_movement_data *movement_data);


    //--------------------------------------------sub_movements preparation---------------------------------------------

public:

    //The function to determine a new sub_movement : nothing particular to do.
    static inline void initialise_sub_movement(k1_sub_movement_data *sub_movement_data) {};

    //The function to update the current variables according to the sub_movement data computed in the function above
    static void accept_sub_movement(k1_sub_movement_data *sub_movement);

    //The first sub_movement preparation, called at the beginning of the movement routine.
    static float compute_us_time_for_first_sub_movement(k1_sub_movement_data *sub_movement_data);

    //The sub_movement preparation function, called on interrupts.
    static float compute_us_time_for_sub_movement(k1_sub_movement_data *sub_movement_data);

    //The evloution factor update function;
    static void update_evolution_coefficient(float multiplier);

private :

    static float sub_movement_time;


    //----------------------------------------------------positon log---------------------------------------------------

public:

    //The function to send the current position through the interface.
    static void send_position();


};


#endif //TRACER_KERNEL1_H

#endif