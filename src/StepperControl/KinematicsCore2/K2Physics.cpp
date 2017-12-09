/*
  RealTimeProcessor.cpp - Part of TRACER

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


#include <StepperControl/SubMovementManager.h>


#include "K2Physics.h"

#include <hardware_language_abstraction.h>
#include <EEPROM/EEPROMStorage.h>
#include <StepperControl/MachineInterface.h>

void K2Physics::start() {

    //init speeds and step_distances to zeros.
    memset(steppers_speeds, 0, NB_STEPPERS * sizeof(float));
    memset(deceleration_distances, 0, NB_STEPPERS * sizeof(uint32_t));

    //Pre compute the regulation_speed constants
    pre_compute_speed_constants();


}


//-----------------------------------------------Speed_Management-------------------------------------------------------

/*
 * TODO
 */

float K2Physics::get_first_sub_movement_time(sub_movement_data_t *sub_movement_data) {

    //Cache vars
    float *f_step_distances = sub_movement_data->f_step_distances;
    float movement_distance = sub_movement_data->movement_distance;
    float speed = sub_movement_data->regulation_speed;

    //The regulation time, corresponding to the regulation regulation_speed;
    float regulation_time = movement_distance / speed;

    //The final time

    float min_time = 0;
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        float maximum_speed = EEPROMStorage::steppers_data[stepper].jerk * EEPROMStorage::steppers_data[stepper].steps;

        //update the minimum time :
        //Formula : low_time_bound = stepper_distance / maximum_speed
        float down_time = f_step_distances[stepper] / maximum_speed;

        //update minimum time, as the maximum of the new time and the current beginning time :
        min_time = (down_time < min_time) ? min_time : down_time;

    }

    float new_time = (regulation_time > min_time) ? regulation_time : min_time;

    last_time = new_time;

    return new_time;

}

/*
 * get_sub_movement_time : this function determines the correct time, for the movement passed in argument.
 *
 *  The movement is passed in he form of :
 *      - movement distance : the high level distance
 *      - stepper_distances : the effective stepper step_distances of the current movement;
 *
 *  It starts to determine the largest time window, at the current speeds. This window is the intersection of
 *      time windows for all steppers.
 *
 *  Finally, the regulation time is determined, according to the following rule :
 *      - if the machine must decelerate_of, the chosen time is the upper bound of the determined window.
 *      - if not, if the regulation time is outside of the regulation window, the new time is the corresponding bound;
 *      - if not, the current time is the regulation time.
 *
 */

float K2Physics::get_sub_movement_time(sub_movement_data_t *sub_movement_data) {

    //Cache vars
    float *f_step_distances = sub_movement_data->f_step_distances;
    float movement_distance = sub_movement_data->movement_distance;
    float speed = sub_movement_data->regulation_speed;


    //The regulation time, corresponding to the regulation regulation_speed;
    float regulation_time = movement_distance / speed;

    //The final time
    float new_time = 0;

    float min_time = 0, max_time = 0;
    bool first = true;
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        //Get the regulation_speed on last movement, on stepper [stepper]
        float act_speed = steppers_speeds[stepper];

        //Get the regulation_speed limit
        //Formula : delta_speed = acceleration * steps * time; acceleration * steps is pre-computed (optimisation)
        float max_delta_speed = delta_speed_constants[stepper] * last_time;

        //if the current stepper can stop, no processing is required :

        float s;

        if ((s = act_speed - max_delta_speed) > 0) {

            //update the maximum time, the new regulation_speed = act_speed - regulation_speed limit.
            //Formula : up_time_bound = stepper_distance / (actual_speed - max_delta_speed)
            float up_time = f_step_distances[stepper] / s;

            //update maximum time, as the minimum of the new time and the current ending time :
            max_time = (first) ? up_time : ((up_time < max_time) ? up_time : max_time);

            first = false;

        }


        float maximum_speed = act_speed + max_delta_speed;
        float maximum_speed_bound = max_speed_constants[stepper];

        if (maximum_speed > maximum_speed_bound) {
            maximum_speed = maximum_speed_bound;
        }

        //update the minimum time :
        //Formula : low_time_bound = stepper_distance / maximum_speed
        float down_time = f_step_distances[stepper] / maximum_speed;

        //update minimum time, as the maximum of the new time and the current beginning time :
        min_time = (down_time < min_time) ? min_time : down_time;


    }
    if ((!first) && (max_time < min_time)) {
        max_time = min_time;
    }
    if ((!first) && (deceleration_required || (regulation_time > max_time))) {
        //If the deceleration is too high, or if the regulation time is higher than the maximum time :

        //Deceleration done
        deceleration_required = false;

        //choose the time corresponding to the maximum deceleration.
        new_time = max_time;

    } else if (regulation_time < min_time) {

        //If the regulation time is lower than the beginning time :

        //choose the time corresponding to the maximum acceleration.
        new_time = min_time;

    } else {


        //If the regulation time is in the time window :

        //choose the regulation time.
        new_time = regulation_time;
    }


    last_time = new_time;

    return new_time;

}


/*
 * update_speeds : this function updates all stepper's speeds.
 *
 */

void K2Physics::update_speeds(sub_movement_data_t *sub_movement_data, float time) {

    //Cache var for stepper distances
    const float *const f_stepper_distances = sub_movement_data->f_step_distances;


    //Only the inverse of time is used, computes now for optimisation purposes.
    float inv_time = 1 / time;

    //Cache vars for end and jerk distances
    int32_t *end_distances = SubMovementManager::end_distances;
    int32_t *jerk_distances = SubMovementManager::jerk_distances;

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        //set the current regulation_speed on the stepper [stepper]

        //get the regulation_speed for the stepper
        //Formula : v = d / t -> v = d * (1 / t).
        float v = steppers_speeds[stepper] = f_stepper_distances[stepper] * inv_time;

        //If a deceleration is required, no need for deceleration_distance checking
        if (deceleration_required)
            continue;

        //Compute the deceleration deceleration_distance on the stepper
        //Formula : v * v / (2 * acceleration * steps); the denominator is pre-computed for optimisation purposes.
        uint32_t deceleration_distance = (uint32_t) (v * v * deceleration_constants[stepper]);

        //get the algebraic end distance.
        int32_t end_distance = end_distances[stepper], jerk_distance = jerk_distances[stepper];

        //get the absolute
        if (end_distance < 0) end_distance = -end_distance;
        if (jerk_distance < 0) jerk_distance = -jerk_distance;

        //require a deceleration if the end distance on the stepper is below the deceleration_distance;
        if (deceleration_distance > (uint32_t) end_distance) {
            deceleration_required = true;
        }

        if (deceleration_distance > (uint32_t) jerk_distance + jerk_offsets[stepper]) {
            deceleration_required = true;
        }
    }
}


/*
 * compute_jerk_offsets : this function computes the jerk offsets caused by the current movement, depending on the
 *      previous movement's ending.
 *
 */

void K2Physics::compute_jerk_offsets(float speed, k2_movement_data *previous_movement) {


    //-----------maximum stepper speeds------------

    //Now that we know the maximum regulation_speed, we can determine the deceleration step_distances :

    //Cache vars for offsets
    uint32_t *jerk_distances_offsets = previous_movement->jerk_offsets;

    //Cache var for ending_jjerk_ratios
    float *ending_jjerk_ratios = previous_movement->ending_jjerk_ratios;

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        //get the maximum regulation_speed on the current axis :
        //Formula : max_stepper_speed = max_speed * stepper_distance / high_level_distance;
        float stepper_speed = speed * ending_jjerk_ratios[stepper];

        stepper_data_t *data = EEPROMStorage::steppers_data + stepper;

        //Get the deceleration distance offset :
        //Formula : the deceleration distance for a stepper at the regulation_speed s (steps/s) is given by
        //      s * s / (2 * acceleration (steps/s^2))
        //TODO PRE-COMPUTATION OPTIMISATION
        jerk_distances_offsets[stepper] = (uint32_t) (stepper_speed * stepper_speed /
                                                      (2 * data->acceleration * data->steps));

    }


}


/*
 * propagate_jerk : this function verifies on each axis that the new jerk parameters do not absorb
 *      previous ones.
 *
 *  If they do, it replaces old ones with new ones.
 *
 */

void K2Physics::propagate_jerk_offsets(const movement_data_t *current_movement, movement_data_t *previous_movement) {

    //Cache pointers
    const int32_t *current_jerk_pos = current_movement->jerk_position;
    int32_t *previous_jerk_pos = previous_movement->jerk_position;
    const uint32_t *current_jerk_offsets = current_movement->jerk_offsets;
    uint32_t *previous_jerk_offsets = previous_movement->jerk_offsets;

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        //get the absolute distance on the axis
        int32_t dist = current_jerk_pos[stepper] - previous_jerk_pos[stepper];
        if (dist < 0) dist = -dist;

        //cache
        uint32_t jd = current_jerk_offsets[stepper];

        //If the new jerk absorbs the old one
        if (previous_jerk_offsets[stepper] + dist < jd) {

            //Replace old offsets
            previous_jerk_offsets[stepper] = jd;

            //Replace old position
            previous_jerk_pos[stepper] = current_jerk_pos[stepper];
        }

    }

}


/*
 * update_jerk_offsets : this function updates the jerk offsets
 *
 */

void K2Physics::update_jerk_offsets(const uint32_t *const new_offsets) {

    memcpy(jerk_offsets, new_offsets, sizeof(uint32_t) * NB_STEPPERS);

}


//---------------------------------------------------Speed_Constants-----------steps-----------------------------------------

void K2Physics::pre_compute_speed_constants() {

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        stepper_data_t *data = EEPROMStorage::steppers_data + stepper;
        float steps = data->steps;
        float as = data->acceleration * steps;
        max_speed_constants[stepper] = data->maximum_speed * steps;
        delta_speed_constants[stepper] = as;
        deceleration_constants[stepper] = (float) 1 / ((float) 2 * as);
    }

}


//-----------------------------------------Static declarations - definitions--------------------------------------------

//Static declarations - definitions;

#define m K2Physics

bool m::deceleration_required = false;

//bool m::jerk_flag = true;

//Global regulation_speed
float m::last_time;


//Steppers speeds
float t_st_spd[NB_STEPPERS];
float *const m::steppers_speeds = t_st_spd;


//Deceleration step_distances
uint32_t t_dec_dst[NB_STEPPERS];
uint32_t *const m::deceleration_distances = t_dec_dst;


//Deceleration step_distances
float t_dec_const[NB_STEPPERS];
float *const m::deceleration_constants = t_dec_const;


//Deceleration step_distances
float t_dsp_const[NB_STEPPERS];
float *const m::delta_speed_constants = t_dsp_const;


//Deceleration step_distances
float t_msp_const[NB_STEPPERS];
float *const m::max_speed_constants = t_msp_const;


//The stepper jerk offsets;
uint32_t k2jot[NB_STEPPERS]{0};
uint32_t *const m::jerk_offsets = k2jot;


#undef m

#endif