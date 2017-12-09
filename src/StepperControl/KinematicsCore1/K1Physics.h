/*
  SpeedManager.h - Part of TRACER

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

#if defined(ENABLE_STEPPER_CONTROL) && (KERNEL==1)

#ifndef TRACER_REAL_TIME_PROCESS_H
#define TRACER_REAL_TIME_PROCESS_H


#include <StepperControl/_kinematics_data.h>
#include <DataStructures/Queue.h>
#include "SqrtFastComputer.h"


class K1Physics {


public:

    static void initialise_tracing_procedure();

    //---------------------------------------------Movement initialisation----------------------------------------------

public :

    static void initialise_kinetics_data(k1_movement_data *movement_data);

    static void load_real_time_kinetics_data(const k1_movement_data *movement_data);

    static void load_real_time_jerk_data(const k1_movement_data *movement_data);

    static void load_pre_process_kinetics_data(const k1_movement_data *movement_data);

    static void update_evolution_coefficient(float multiplier);


private :

    static uint8_t reference_axis;

    static uint8_t last_speed_group;

    static float evolution_coefficient;

    static float saved_evolution_coefficient;

    static float last_movement_first_sub_movement_hl_distances;

    static float current_movement_first_sub_movement_hl_distances;


    //---------------------------------------------Movement initialisation----------------------------------------------

private:

    static void get_delay_numerator_data(k1_movement_data *movement_data);

    static uint8_t get_distances(const float *const t0, float *const t1);

    static float get_delay_numerator(void (*trajectory_function)(float, float*), float p0, float p1, uint8_t *max_axis, float *sav_max_distance);

    static float _get_delay_numerator(uint8_t stepper, float distance);


    //---------------------------------------------Movement initialisation----------------------------------------------

public :

    //function to compute the first sub movement time. Called at the first routine iteration.
    static float get_first_sub_movement_time(k1_sub_movement_data *sub_movement_data);

private:

    //The function to pre-compute a movement'ssub_movement time.
    static void get_sub_movement_time(movement_data_t *movement_data, uint8_t speed_group, float speed);


    //-----------------------------------------------End distance-------------------------------------------------

public :

    static void update_heuristic_end_distance();

private:

    //The heuristic distance to the end point;
    static uint32_t heuristic_end_distance;


    //------------------------------------------------------Jerk--------------------------------------------------------

public:

    static void update_heuristic_jerk_distance();

    //Jerk offsets computation
    static void compute_jerk_offsets(float speed, k1_movement_data *previous_movement);

    //Jerk offsets propagation
    static void propagate_jerk_offsets(const movement_data_t *current_movement, movement_data_t *previous_movement) {};


private:

    //The stepper jerk offset;
    static uint32_t jerk_distance_offset;

    //The offset heuristic distance to the next jerk point
    static uint32_t offset_heuristic_jerk_distance;

    //The jerk flag : if true, the jerk point proximity is checked in real time
    static volatile bool watch_for_jerk_point;


    //------------------------------------------------Speed_Management--------------------------------------------------

public :

    static bool regulate_speed();

    static float get_sub_movement_time(bool update);


private:

    //speed distance management

    static void decelerate_to(uint32_t distance);

    static void accelerate_of(float elapsed_distance);

    static void decelerate_of(float elapsed_distance);

    static void update_acceleration_distances();

    static void update_deceleration_distances();


private :

    //The acceleration_step
    static float acceleration_step;

    //The acceleration distances's square_root
    static uint16_t acceleration_speed_distance_sqrt;

    //The speed distances
    static float deceleration_speed_distance;
    static float acceleration_speed_distance;


    //Delay numerator
    static float acceleration_delay_numerator;

    //The acceleration distance sqrt fast computer
    static SqrtFastComputer acceleration_distance_computer;

    //The distances ratios.
    static float deceleration_to_acceleration;
    static float acceleration_to_deceleration;

    //The current sub_movement time
    static delay_t sub_movement_time;

    //The current regulation (target) sub_movement time
    static delay_t regulation_sub_movement_time;

    //A flag to mention if the tracing routine has just begun.
    static bool first_movement;

    //A flag to mention if the speed currently increases
    static bool speed_increasing_flag;

    //A flag to disable the regulation for the current sub_movement.
    static bool no_regulation_flag;

    //A flag to enable the speed regulation
    static bool speed_regulation_enabled;


};

#endif //TRACER_REAL_TIME_PROCESS_H

#endif