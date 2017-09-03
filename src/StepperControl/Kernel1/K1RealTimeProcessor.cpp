/*
  SpeedManager.cpp - Part of TRACER

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

#ifdef ENABLE_STEPPER_CONTROL

#include <math.h>
#include <WString.h>
#include <Actions/ContinuousActions.h>
#include <hardware_language_abstraction.h>
#include <interface.h>
#include "K1RealTimeProcessor.h"
#include "SpeedPlanner.h"
#include "mathProcess.hpp"



/*
 * TODO
 */

void K1RealTimeProcessor::begin() {
    set_speed_distance(0);
}


/*
 * TODO
 */

void K1RealTimeProcessor::update_end_jerk_distances(const sig_t negative_signatures, const uint8_t *elementary_dists) {

#define STEPPER(i, sig, ...)\
        if (negative_signatures&sig) {end_distances[i] += elementary_dists[i];jerk_distances[i] += elementary_dists[i];}\
        else {end_distances[i] -= elementary_dists[i];jerk_distances[i] -= elementary_dists[i];}\


#include <config.h>

#undef STEPPER

    update_heuristic_end_distance();
    update_heuristic_jerk_distance();

}


/*
 * TODO
 */

void K1RealTimeProcessor::update_heuristic_end_distance() {

    int32_t td;
    uint32_t distance = 0;
    uint32_t dist = 0;

#define STEPPER(i, ...) \
    td = end_distances[i];\
    distance=((uint32_t) (td<0) ? -td : td );\
    if (distance>dist) dist = distance;

#include <config.h>

#undef STEPPER

    heuristic_end_distance = dist;

}

/*
 * TODO
 */

void K1RealTimeProcessor::update_heuristic_jerk_distance() {

    int32_t td;
    uint32_t distance = 0;
    uint32_t dist = 0;

#define STEPPER(i, ...) \
    td = jerk_distances[i];\
    distance=((uint32_t) (td<0) ? -td : td );\
    if (distance>dist) dist = distance;

#include <config.h>

#undef STEPPER

    offseted_heuristic_jerk_distance = dist + jerk_offset;

}


/*
 * TODO
 */

void K1RealTimeProcessor::regulate_speed(uint8_t elapsed_distance) {

    if (heuristic_end_distance < speed_distance) {
        //If the machine must decelerate, due to the proximity of the end point :

        //decelerate
        set_speed_distance(heuristic_end_distance);

        //Re-enable the regulation
        speed_regulation_enabled = true;

        //setup the regulation for an acceleration
        speed_increasing = true;

        //Disable the
        no_regulation_flag = true;

        return;

    } else if ((watch_for_jerk_point) && (offseted_heuristic_jerk_distance < speed_distance)) {
        //If the machine must decelerate, due to the proximity of the jerk point :

        //decelerate
        set_speed_distance(offseted_heuristic_jerk_distance);

        //Re-enable the regulation
        speed_regulation_enabled = true;

        //setup the regulation for an acceleration
        speed_increasing = true;

        //Disable the
        no_regulation_flag = true;

        return;

    } else if (speed_regulation_enabled) {
        //If speed still has to be modified :

        //If we decelerated during the last sub_movement, we do not correct the speed for this one.
        if (no_regulation_flag) {

            //disable the flag for the next sub_movement
            no_regulation_flag = false;
            return;

        }

        bool low_speed = (delay0 > regulation_delay);

        //We must verify that the target hasn't been reached
        //Reached when the speed is too high and has been increased before, or too low and has been decreased before
        if (low_speed != speed_increasing) {

            //Set the delay to the target
            delay0 = regulation_delay;

            //mark the target as reached
            speed_regulation_enabled = false;
            return;

        } else if (low_speed) {
            //if speed is still too low :

            //acceleration
            accelerate(elapsed_distance);

            //mark the speed as increasing
            speed_increasing = true;

            return;
        } else {
            //if speed is still too high :

            //deceleration
            decelerate(elapsed_distance);

            //mark the speed as decreasing
            speed_increasing = false;

            return;
        }
    }
}


/*
 * TODO
 */

void K1RealTimeProcessor::set_speed_distance(uint32_t distance) {
    speed_distance = distance;
    update_distance_sqrt();
}


/*
 * TODO
 */

void K1RealTimeProcessor::accelerate(uint8_t elapsed_distance) {
    speed_distance += elapsed_distance;
    update_distance_sqrt();
}


/*
 * TODO
 */

void K1RealTimeProcessor::decelerate(uint8_t elapsed_distance) {
    speed_distance -= elapsed_distance;
    update_distance_sqrt();

}


/*
 * TODO
 */

void K1RealTimeProcessor::update_distance_sqrt() {
    distance_square_root = distance_sqrt_computer.sqrt_fast(speed_distance);

}


float last_ratio;

/*
 * init_speed_management : this function is called before a movement is actually executed.
 *
 *  It uses the 3 parameters pre-computed during the motion scheduling by SpeedPlanner::pre_set_speed_axis :
 *      - ratio : the distance ratio, used to adapt the speed management to one axis, in linear moves (1 for others);
 *      - tmp_delay_numerator : the new delay numerator;
 *      - tmp_regulation_delay : the new regulation delay;
 */
//TODO MODIFIER LE JERK_OFFSET A CHAQUE MOUVEMENT
void K1RealTimeProcessor::init_speed_management(delay_t tmp_regulation_delay, delay_t tmp_delay_numerator,
                                                float tmp_speed_factor, float ratio,
                                                bool jerk_point,
                                                uint32_t jerk_distance_offset) {
    //Set primary and delay0
    delay_t tmp_delay_0;


    if (speed_distance != 0) {
        //If the machine is moving

        //Update the delay
        tmp_delay_0 = (delay_t) ((float) delay0 * ratio / last_ratio);

        //Update the speed_distance square_root
        uint32_t sqrt_new_speed_distance = (uint32_t) ((float) tmp_delay_numerator / (float) tmp_delay_0);

        //Update the speed_distance
        set_speed_distance(sqrt_new_speed_distance * sqrt_new_speed_distance);

    } else {

        //if not, simply take the minimum delay
        tmp_delay_0 = tmp_delay_numerator;

    }

    last_ratio = ratio;
    delay0 = tmp_delay_0;

    set_stepper_int_period(tmp_delay_0);
    delay_numerator = tmp_delay_numerator;
    regulation_delay = tmp_regulation_delay;
    speed_factor = tmp_speed_factor;
    jerk_offset = jerk_distance_offset;

    speed_regulation_enabled = true;

    if (jerk_point) {
        SpeedPlanner::pull_jerk_point();
    } else {
        watch_for_jerk_point = SpeedPlanner::must_watch_for_jerk_points();
    }

}


void K1RealTimeProcessor::update_delay_0() {
    delay0 = (delay_t) (delay_numerator * invert(distance_square_root));
    delay0_update_required = false;
}


#define m K1RealTimeProcessor

volatile bool m::watch_for_jerk_point = false;

uint32_t m::speed_distance = (uint32_t) 0;
uint32_t m::offseted_heuristic_jerk_distance = 0;
uint32_t m::heuristic_end_distance = 0;

bool m::speed_increasing = true;
bool m::no_regulation_flag = false;
bool m::speed_regulation_enabled = false;

delay_t m::regulation_delay;
delay_t m::delay0;
delay_t m::delay_numerator;
float m::speed_factor;

bool m::delay0_update_required;

uint32_t m::jerk_offset;

//End distances
int32_t ted[NB_STEPPERS]{0};
int32_t *const m::end_distances = ted;

//jerk distances
int32_t tjd[NB_STEPPERS]{0};
int32_t *const m::jerk_distances = tjd;

uint8_t m::linear_tools_nb;

void (*tf[NB_CONTINUOUS]);

void (**m::linear_set_functions)(float) = (void (**)(float)) tf;


#undef m


#endif