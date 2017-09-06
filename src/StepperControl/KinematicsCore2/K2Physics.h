/*
  RealTimeProcessor.h - Part of TRACER

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

#if defined(ENABLE_STEPPER_CONTROL) && (KERNEL == 2)

#ifndef TRACER_REAL_TIME_PROCESS_H
#define TRACER_REAL_TIME_PROCESS_H

#include <DataStructures/Queue.h>
#include <StepperControl/_kinematics_data.h>

class K2Physics {

public:

    static void start();



    //------------------------------------------------Speed_Management--------------------------------------------------

public :

    //the function to compute the time for the first sub_movement of the routine
    static float get_first_sub_movement_time(sub_movement_data_t *sub_movement_data);

    //the function to compute the time for the current sub_movement.
    static float get_sub_movement_time(sub_movement_data_t *sub_movement_data);

    //the function to update the current regulation_speed of the steppers, knowing the current movement time
    static void update_speeds(sub_movement_data_t *sub_movement_data, float time);


    //--------------------------------------------------Jerk offsets----------------------------------------------------

    //Jerk offsets computation
    static void compute_jerk_offsets(float speed, k2_movement_data *previous_movement);

    //Jerk offsets propagation
    static void propagate_jerk_offsets(const k2_movement_data *current_movement, k2_movement_data *previous_movement);

    //Jerk offsets update
    static void update_jerk_offsets(const uint32_t *const new_jerk_offsets);


private :

    //Deceleration Fields,  computed during the heuristic calls
    static bool deceleration_required;

    //Previous sub_movement duration
    static float last_time;

    //Current regulation_speed of steppers
    static float *const steppers_speeds;

    //Current steppers deceleration step_distances
    static uint32_t *const deceleration_distances;

    //The stepper jerk offsets;
    static uint32_t *const jerk_offsets;


    //-------------------------------------------------Speed_Constants--------------------------------------------------

private :

    //The function to actualise the movement constants
    static void pre_compute_speed_constants();

    /*
     * The array containing the deceleration constants : in the deceleration distance formula :
     *      (((v * v) / (2 * EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper]));
     *
     *  the denominator is constant. This array wil contain the float value
     *      1.0 / (2.0 * EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper]));
     *
     *      for each stepper.
     */

    static float *const deceleration_constants;


    /*
     * The array containing the delta regulation_speed constants : in the deceleration distance formula :
     *      max_delta_speed = EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper] * time;
     *
     *  the product of the two first terms is constant. This array wil contain the float value
     *      EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper]
     *
     *      for each stepper.
     */

    static float *const delta_speed_constants;

    /*
     * The array containing the maximum regulation_speed constants : in the deceleration distance formula :
     *      max_speed = EEPROMStorage::speeds[stepper] * EEPROMStorage::steps[stepper] * time;
     *
     *  the product is constant. This array wil contain the float value
     *      EEPROMStorage::speeds[stepper] * EEPROMStorage::steps[stepper]
     *
     *      for each stepper.
     */

    static float *const max_speed_constants;


};


#endif //TRACER_REAL_TIME_PROCESS_H

#endif