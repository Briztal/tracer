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
#include "SpeedManager.h"
#include "SpeedPlanner.h"
#include "mathProcess.hpp"


void SpeedManager::begin() {
    set_speed_distance(0);
}

void SpeedManager::updateActions() {
    if (ContinuousActions::linear_modes_enabled()) {
        linear_tools_nb = ContinuousActions::getSetFunctions(linear_set_functions);
    } else {
        linear_tools_nb = 0;
    }
}

/*
 * Heuristic distance : this function calculates the heuristic distance between the actual position and the end position
 *
 * The current heuristic is the sum of all distances.
 */
void SpeedManager::heuristic_end_distance() {

    int32_t td;
    uint32_t distance = 0;
    uint32_t dist = 0;

#define STEPPER(i, ...) \
    td = end_distances[i];\
    distance=((uint32_t) (td<0) ? -td : td );\
    if (distance>dist) dist = distance;

#include <config.h>

#undef STEPPER

    distance_to_end_point = dist;

}

void SpeedManager::heuristic_jerk_distance() {

    int32_t td;
    uint32_t distance = 0;
    uint32_t dist = 0;

#define STEPPER(i, ...) \
    td = jerk_distances[i];\
    distance=((uint32_t) (td<0) ? -td : td );\
    if (distance>dist) dist = distance;

#include <config.h>

#undef STEPPER

    offseted_distance_to_jerk_point = dist + speed_offset;


}


/*
 * regulate_speed : this function uses the distance to the end provided by the heuristic, to modify the primary.
 *
 * It the distance is less than the minimal deceleration distance to respect the acceleration limit,
 *      it decrements the speed distance, and so enables the speed modification.
 *
 * If not, if the regulation speed has not been reached, it increments (or decrements) the speed distance,
 *      to go forward to it.
 */

void SpeedManager::regulate_speed() {
    //CI::echo("DISTANCE TO JERK POINT : "+String(primary)+" "+String(distance_to_end_point)+" "+String(offseted_distance_to_jerk_point)+" "+String(speed_offset));

    if (distance_to_end_point < speed_distance) {

        //If we entered in the deceleration range :
        go_to_speed_distance(distance_to_end_point);
        regulation_unreached = true;
        regulation_stop_enabled = false;
        return;
    } else if ((watch_for_jerk_point)&&(offseted_distance_to_jerk_point < speed_distance)) {

        //If we entered in the deceleration range :
        go_to_speed_distance(offseted_distance_to_jerk_point);
        regulation_unreached = true;
        regulation_stop_enabled = false;
        return;

    } else if (regulation_unreached) {
        //If speed still has to be modified :

        if (delay0 > regulation_delay) {
            //if speed is too low :

            if (!speed_incr && regulation_stop_enabled) {
                //If last call reduced the speed : theorical_regulation_speed is reached
                delay0 = regulation_delay;
                regulation_unreached = false;
                return;
            }
            //if not, acceleration
            set_speed_distance_fast(true, processing_steps);
            speed_incr = true;
            regulation_stop_enabled = true;

            return;
        } else if (delay0 < regulation_delay) {
            //if speed is too high :

            if (speed_incr && regulation_stop_enabled) {
                //If last call increased the speed : theorical_regulation_speed is reached

                delay0 = regulation_delay;
                regulation_unreached = false;
                return;
            }
            //if not, deceleration
            set_speed_distance_fast(false, processing_steps);
            speed_incr = false;
            regulation_stop_enabled = true;

            return;
        }
    }
}


/*
 * sqrt_slow : this function updates the speed_distances, and its square root, with a call to sqrt.
 *
 * It also sets correct variables for the fast algorithm.
 */
void SpeedManager::set_speed_distance(uint32_t distance_to_end) {
    speed_distance = distance_to_end;
    distance_square_root = (uint16_t) sqrt(distance_to_end);
    square_sup = square_increments = (uint16_t) (2 * distance_square_root + 1);
    square_inf = 1;
    delay0_update_required = true;
}


/*
 * _sqrt_fast : this function updates the primary and its square root,
 *      with a more efficient algorithm than calling sqrt
 *
 * If positive_incr is true, it increments primary with incr value. If not, it decrements it with incr value.
 *
 * This algorithm is based on the formula n*n = 1 + 3 + 5 + ... + (2.n-1)
 *
 * When the square root is modfied, it sets delay0_update_required, so that delay (speed) will be modified aferwards.
 *
 * The delay is set according to  : delay = delay_numerator/sqrt(primary)
 *
 */
void SpeedManager::set_speed_distance_fast(bool positive_incr, uint16_t incr) {

#define switch_up square_inf = 1, square_increments+=2, square_sup = square_increments, distance_square_root++, delay0_update_required = true;
#define switch_down if(distance_square_root) { square_increments-=2, square_inf=square_increments, square_sup=1, distance_square_root--;delay0_update_required = true;}

    if (positive_incr) {
        //An increment is required :

        //incrementing the distance
        speed_distance += incr;

        //Updating bounds and square root
        while (square_sup < incr) {
            incr -= square_sup;
            switch_up
        }

        //last update, with the reduced increment
        square_sup -= incr, square_inf += incr;
    } else {
        //An decrement is required :

        if (speed_distance > incr) {
            //If we need to decrement less than the speed distance

            //decrementing the distance
            speed_distance -= incr;

            //Updating bounds and square root
            while (square_inf < incr) {
                incr -= square_inf;
                switch_down
            }

            //last update, with the reduced increment
            square_sup += incr, square_inf -= incr;
            return;
        } else {
            //If we need to decrement more than the speed distance : speed distance goes to zero (necessarily >= 0);

            speed_distance = 0;
            square_inf = square_sup = square_increments = 1;
            distance_square_root = 0;

        }
    }

#undef switch_down
#undef switch_up
}

/*
 * sqrt_fast : this function sets speed distance, in absolute mode.
 *
 * It calls _sqrt_fast in relative mode.
 */

void SpeedManager::go_to_speed_distance(uint32_t distance_to_end) {

    int32_t d = distance_to_end - speed_distance;

    bool positive_incr;
    if (!(positive_incr = d > 0)) d = -d;

    set_speed_distance_fast(positive_incr, (uint16_t) d);

}

void SpeedManager::print_speed_distance() {
    CI::echo("sd : " + String(speed_distance));
    CI::echo("de : " + String(distance_square_root));
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
void SpeedManager::init_speed_management(delay_t tmp_regulation_delay, delay_t tmp_delay_numerator,
                                         float tmp_speed_factor, float ratio, uint8_t processing_steps, bool jerk_point,
                                         uint32_t jerk_distance_offset) {
    //Set primary and delay0
    delay_t tmp_delay_0;
    if (speed_distance != 0) {
        tmp_delay_0 = (delay_t) ((float) delay0 * ratio / last_ratio);
        uint32_t sqrt_new_speed_distance = (uint32_t) ((float) tmp_delay_numerator / (float) tmp_delay_0);
        go_to_speed_distance(sqrt_new_speed_distance * sqrt_new_speed_distance);
    } else {
        tmp_delay_0 = tmp_delay_numerator;
    }

    last_ratio = ratio;
    delay0 = tmp_delay_0;

    set_stepper_int_period(tmp_delay_0);
    delay_numerator = tmp_delay_numerator;
    regulation_delay = tmp_regulation_delay;
    speed_factor = tmp_speed_factor;
    speed_offset = jerk_distance_offset;

    regulation_unreached = true;
    SpeedManager::processing_steps = processing_steps;

    if (jerk_point) {
        SpeedPlanner::pull_jerk_point();
    } else {
        watch_for_jerk_point = SpeedPlanner::must_watch_for_jerk_points();
    }

}

void SpeedManager::setActionsSpeeds() {

    float temp_speed = speed_factor * distance_square_root;

    for (uint8_t action_index = linear_tools_nb; action_index--;) {
        linear_set_functions[action_index](temp_speed);
    }
}


void SpeedManager::update_delay_0() {
    delay0 = (delay_t) (delay_numerator * invert(distance_square_root));
    delay0_update_required = false;
}

#define m SpeedManager

volatile bool m::watch_for_jerk_point = false;

uint32_t m::speed_distance = (uint32_t) 0;
uint32_t m::offseted_distance_to_jerk_point = 0;
uint32_t m::distance_to_end_point = 0;
uint16_t m::square_inf = 1, m::square_sup = 1, m::distance_square_root = 0, m::square_increments = 1;

bool m::speed_incr = true;
bool m::regulation_stop_enabled = false;
bool m::regulation_unreached = false;

delay_t m::regulation_delay;
delay_t m::delay0;
delay_t m::delay_numerator;
float m::speed_factor;

bool m::delay0_update_required;

uint8_t m::processing_steps;

uint32_t m::speed_offset;

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