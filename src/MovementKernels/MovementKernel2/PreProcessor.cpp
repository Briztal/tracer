/*
  CSpeedManager.cpp - Part of TRACER

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


#include "../../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#include "PreProcessor.h"
#include "../../Actions/ContinuousActions.h"
#include "../../interface.h"
#include "../../Core/EEPROMStorage.h"
#include "complex_motion_data.h"
#include "../StepperAbstraction.h"

void PreProcessor::updateActions() {
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
void PreProcessor::heuristic_end_distance() {

    int32_t td;
    uint32_t distance = 0;
    uint32_t dist = 0;

#define STEPPER(i, ...) \
    td = end_distances[i];\
    distance=((uint32_t) (td<0) ? -td : td );\
    if (distance>dist) dist = distance;

#include "../../config.h"

#undef STEPPER

    distance_to_end_point = dist;

}

void PreProcessor::heuristic_jerk_distance() {

    int32_t td;
    uint32_t distance = 0;
    uint32_t dist = 0;

#define STEPPER(i, ...) \
    td = jerk_distances[i];\
    distance=((uint32_t) (td<0) ? -td : td );\
    if (distance>dist) dist = distance;

#include "../../config.h"

#undef STEPPER

    offseted_distance_to_jerk_point = dist + speed_offset;

}



//-----------------------------------Intermediary_Positions_Pre_Computation---------------------------------------------

/*
 * push_new_position : this function gets the next position, defined by the image of index+increment
 *      by the trajectory function.
 *
 *      If the image verifies one of the criteria below :
 *          - at a distance from the current position comprised on MINIMUM_DISTANCE_LIMIT and MAXIMUM_DISTANCE_LIMIT
 *          - at a distance from the current position that equals DISTANCE_TARGET
 *
 *      Then the position is accepted and enqueued in the position queue.
 *
 *      If not, the position is discarded.
 *
 *      In all cases, the increment is updated to converge to the distance target.
 *
 *      Notes :
 *          - The distance is given by the maximum of all distances on each axis.
 *          - The second criteria is (far) more restrictive than the first, but gives a better speed regulation
 *
 *          //TODO UPDATE THE DOC WHEN THE CODE IS DONE
 */
void PreProcessor::push_new_position() {

    if (last_position_processed)
        return;

    //High level positions : the new position in the high level system (will be provided by get_new_positions).
    float high_level_positions[NB_AXIS];

    //Stepper positions : the new position in the steppers system (translated by StepperAbstraction).
    int32_t steppers_positions[NB_STEPPERS];


    //Stepper distances : As copying requires time, we directly put the temp distances in the final array.
    uint8_t push_indice = sub_movement_queue.push_indice();
    uint8_t *elementary_dists = sub_movement_distances + push_indice * NB_STEPPERS;

    //The maximal stepper distance, and the maximum axis
    uint8_t max_distance, max_axis;

    sig_t negative_signature;

    //------------------Distances_computing-----------------------------

    //Get the new index candidate;
    float index_candidate = index + increment;

    if (index_candidate + index_candidate > index_max) {
        last_position_processed = true;
        index_candidate = index_max;
    }

    //Get the new high level position;
    get_new_position(index_candidate, high_level_positions);

    //Translate the high level position into steppers position;
    StepperAbstraction::translate(high_level_positions, steppers_positions);

    //Get the steppers distances, and the maximal axis and distance
    bool up_check = get_distances(current_stepper_positions, steppers_positions, elementary_dists,
                                  &negative_signature, &max_axis, &max_distance);


    //-----------------Validity_Verification----------------------------

    if (max_distance != DISTANCE_TARGET) {

        //Increment adjustment according to the target
        increment = increment * (float) DISTANCE_TARGET / (float) max_distance;
        //TODO : if the queue is full, discard the current move
    }

    //If the maximal distance is below the lower limit :
    if (up_check) {
        last_position_processed = false;
        return;
    }

    //If the maximal distance is below the lower limit :
    if ((!last_position_processed) && (max_distance <= MINIMUM_DISTANCE_LIMIT)) {
        return;
    }


    //---------------------Movement_Enqueuing----------------------------

    /*
     * Now that validity checks are made, we can enqueue the sub_movement :
     * As the distances are already in the array, all we need to do is to push the direction signature
     *      and validate the candidate index.
     */

    sub_movement_queue.push(negative_signature);

    index = index_candidate;

}


/*
 * get_distances : this function determines the distance between a position and a target, for all axis.
 *
 * It also computes the direction signature, as distances are positive numbers, and saves the maximum axis
 *      and the maximum distance.
 *
 */

bool
PreProcessor::get_distances(const int32_t *const pos, const int32_t *const dest,
                            uint8_t *const dists, sig_t *dir_dignature_p,
                            uint8_t *max_axis_p, uint8_t *max_distance_p) {

    //Cache variable, to avoid pointer access.
    uint8_t max_dist = 0;
    uint8_t max_axis = 0;
    sig_t dir_signature = 0;

    //We must determine the distance for every axis
    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {

        //get distance on axis
        int32_t distance = pos[axis] - dest[axis];

        //get absolute distance
        if (distance < 0) {
            distance = -distance;
            dir_signature |= SpeedPlanner::axis_signatures[axis];
        }

        //if the distance is greater than the limit : fail with true
        if (distance >= MAXIMUM_DISTANCE_LIMIT) {
            return true;
        }



        //cast to small type
        uint8_t rdist = (uint8_t) distance;

        //persist distance
        dists[axis] = rdist;

        //Update max dist
        if (rdist > max_dist) {
            max_dist = rdist;
            max_axis = axis;
        }
    }

    //Finally update all data
    *max_axis_p = max_axis;
    *max_distance_p = max_dist;
    *dir_dignature_p = dir_signature;

    //No error
    return false;
}


//-----------------------------------------------Speed_Management-------------------------------------------------------

float PreProcessor::pre_process_speed(float movement_distance, const uint8_t *const stepper_distances) {


    if (deceleration_required) {
        //If the machine must decelerate, because of the proximity of a jerk/end point:

        //temp value for the deceleration axis.
        uint8_t axis = deceleration_axis;

        //the value sqrt(2*acceleration*stepspmm)
        float sqrt_2as = (float) sqrt(
                2 * EEPROMStorage::accelerations[axis] * EEPROMStorage::steps[axis]);//TODO PRE_PROCESS

        //The square root of the deceleration distance
        float sqrt_dist = (float) sqrt(deceleration_dist); //TODO FASTSQRT

        //The delay time
        float time = (float) stepper_distances[axis] / (sqrt_2as * sqrt_dist);

        //Update the speed
        current_speed = movement_distance / time;

        return time;

    } else if (acceleration_required) {
        //If the regulation speed hasn't been reached yet :

        float min_time = 0;

        /*
         * We will here determine the minimum time for the current movement.
         *
         *  To do this, we will calculate the min time on each axis and determine the minimum of those.
         *
         *  After, we update the current speed
         *
         */

        for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {

            //get the current speed on the stepper [axis]
            float act_speed = steppers_speeds[axis];

            //calculate the minimum time, according to the acceleration bound :
            float time = stepper_distances[axis] /
                         (act_speed + EEPROMStorage::accelerations[axis] * EEPROMStorage::steps[axis]);

            min_time = (time < min_time) ? min_time : time;

        }

        //Update the speed
        current_speed = movement_distance / min_time;

        return min_time;


    } else {
        //If the machine is at its regulation speed : verify the acceleration/deceleration bounds.


        float time = movement_distance / current_speed;

        float inv_time = 1 / time;

        float min_time = 0, max_time = 0;

        bool first_max = true;

        bool max_reached = false, min_reached = false;

        //Get the time bounds for each axis;

        for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
            //get the current speed on the stepper [axis]

            float act_speed = steppers_speeds[axis];

            float next_speed = (float) stepper_distances[axis] * inv_time;

            float speed_diff = next_speed - act_speed;

            bool in_deceleration = (speed_diff > 0);

            if (in_deceleration) speed_diff = -speed_diff;

            float speed_limit = EEPROMStorage::accelerations[axis] * EEPROMStorage::steps[axis];

            //Acceleration or Deceleration limit reaching :
            if (speed_diff > speed_limit) {

                if (in_deceleration) {

                    //if we decelerate, the new speed = act_speed - speed limit.
                    float n_time = stepper_distances[axis] / (act_speed - speed_limit);

                    //update maximum time, as the minimum of the new time and the current max time :
                    min_time = (first_max) ? n_time : ((n_time < min_time) ? min_time : n_time);

                    first_max = false;

                    max_reached = true;

                } else {

                    //if we accelerate, the new speed = act_speed + speed limit.
                    float n_time = stepper_distances[axis] / (act_speed + speed_limit);

                    //update minimum time, as the maximum of the new time and the current min time :
                    max_time = (n_time < max_time) ? n_time : max_time;

                    min_reached = true;
                }

            }

        }

        float new_time = 0;

        if (max_reached) {
            //If the deceleration is too high, or if both limits are reached (impossible move) : decelerate.

            new_time = max_time;

        } else if (min_reached) {
            //If the acceleration is too high : accelerate.

            new_time = min_time;

        } else {
            //If all is normal, do not change time.

            new_time = time;
        }

        //Update the speed
        current_speed = movement_distance / new_time;

        return new_time;

    }

}


void PreProcessor::update_speeds(const uint8_t *const stepper_distances, float time) {

    float inv_time = 1 / time;

    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        //set the current speed on the stepper [axis]

        float v = steppers_speeds[axis] = (float) stepper_distances[axis] * inv_time;

        deceleration_distances[axis] = (v*v)/(2*EEPROMStorage::accelerations[axis]);//TODO PRE_PROCESS

    }
}


//----------------------------------------------------pop---------------------------------------------------------------



/*
 * pop_next_position : this function pops a previously enqueued position.
 *
 *      It returns an error if the queue is empty.
 *
 *  It copies the distances in the provided array, and returns the negative signature assaoiated with those distances.
 *
 */

void PreProcessor::pop_next_position(uint8_t *elementary_dists, sig_t *negative_signature, float *distance) {

    uint8_t size = sub_movement_queue.available_elements();

    if (!size) {

        //if the queue is empty : error
        CI::echo("ERROR : IN ComplexTrajectoryExecuter::pop_next_position : THE SUB_MOVEMENT QUEUE IS EMPTY");

        return;

    } else {

        //If the queue only contains the last sub_movement : disable the sub_movement pre_processing
        if ((size == 1) && (last_position_processed)) {
            last_position_popped = true;
        }

        //The pointer to the beginning of pre-processed elementary distances
        uint8_t *ptr = sub_movement_distances + NB_STEPPERS * sub_movement_queue.pull_indice();

        //copy
        memcpy(elementary_dists, ptr, NB_STEPPERS * sizeof(uint8_t));

        //update the direction signature and the distance memorised in the queue.
        pre_processor_data data = sub_movement_queue.pull();

        *negative_signature = data.negative_signature;
        *distance = data.distance;

        return;

    }

}

void PreProcessor::reset_vars() {
    PreProcessor::last_sub_move_engaged = PreProcessor::last_position_popped = PreProcessor::last_position_processed = false;
}


#define m PreProcessor

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

sig_t (*m::get_new_position)(float, float *);


#undef m


#endif