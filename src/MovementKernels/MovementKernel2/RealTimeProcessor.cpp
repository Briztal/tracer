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


#include "../../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#include "RealTimeProcessor.h"
#include "../../Actions/ContinuousActions.h"
#include "../../interface.h"
#include "../../Core/EEPROMStorage.h"
#include "complex_motion_data.h"
#include "../StepperAbstraction.h"


void RealTimeProcessor::start() {


    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        //initialise speeds and distances

        steppers_speeds[stepper] = 0;

        deceleration_distances[stepper] = 0;

        //TODO PRE_PROCESSES

    }

}

void RealTimeProcessor::set_regulation_speed(float speed) {
    regulation_speed = speed;
}

/*
 * initialise_movement : this function is called when the current movement is finished.
 *
 *  Parameters passed update all variables used to compute positions :
 *      - min and max, the minimum and the maximum of index (beginning and ending of movement)
 *      - incr : the index increment for the first prepare_movement (pre computed and supposedly accurate)
 *      - trajectory_function : the function that actually computes stepper positions.
 *
 */

void
RealTimeProcessor::initialise_movement(float min, float max, float incr, void (*trajectory_function)(float, float *)) {

    //update trajectory extrmas, index, and increment
    index_min = min;
    index_max = max;
    index = min;
    increment = incr;

    //update position provider
    get_new_position = trajectory_function;

}


void RealTimeProcessor::updateActions() {
    if (ContinuousActions::linear_modes_enabled()) {
        linear_tools_nb = ContinuousActions::getSetFunctions(linear_set_functions);
    } else {
        linear_tools_nb = 0;
    }
}


//-----------------------------------Intermediary_Positions_Pre_Computation---------------------------------------------


void RealTimeProcessor::fill_sub_movement_queue() {
    while (sub_movement_queue.available_spaces() || last_position_processed) {
        push_new_position();
    }
}

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
 *          - The distance is given by the maximum of all distances on each stepper.
 *          - The second criteria is (far) more restrictive than the first, but gives a better speed regulation
 *
 *          //TODO UPDATE THE DOC WHEN THE CODE IS DONE
 */

void RealTimeProcessor::push_new_position() {

    if (last_position_processed)
        return;

    //High level positions : the new position in the high level system (will be provided by get_new_positions).
    float high_level_positions[NB_AXIS];

    //Stepper positions : the new position in the steppers system (translated by StepperAbstraction).
    int32_t steppers_positions[NB_STEPPERS];


    //Stepper distances : As copying requires time, we directly put the temp distances in the final array.
    uint8_t push_indice = sub_movement_queue.push_indice();
    uint8_t *elementary_dists = sub_movement_distances + push_indice * NB_STEPPERS;

    //The maximal stepper distance, and the maximum stepper
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

    float movement_distance = get_hl_distance(high_level_positions);

    //Translate the high level position into steppers position;
    StepperAbstraction::translate(high_level_positions, steppers_positions);

    //Get the steppers distances, and the maximal stepper and distance
    bool up_check = get_steppers_distances(current_stepper_positions, steppers_positions, elementary_dists,
                                           &negative_signature, &max_axis, &max_distance);


    //-----------------Validity_Verification----------------------------

    if (max_distance != DISTANCE_TARGET) {

        //Increment adjustment according to the target
        increment = increment * (float) DISTANCE_TARGET / (float) max_distance;
        //TODO : if the queue is full, discard the current prepare_movement
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

    pre_processor_data *d = sub_movement_queue.get_push_ptr();

    d->negative_signature = negative_signature;
    d->distance = movement_distance;
    if (next_push_jerk) {
        d->jerk_point = true;
        d->speed = next_regulation_speed;
    } else {
        d->jerk_point = false;
    }

    sub_movement_queue.push();

    index = index_candidate;

}


/*
 * get_steppers_distances : this function determines the distance between a position and a target, for all stepper.
 *
 * It also computes the direction signature, as distances are positive numbers, and saves the maximum stepper
 *      and the maximum distance.
 *
 */

bool
RealTimeProcessor::get_steppers_distances(const int32_t *const pos, const int32_t *const dest,
                                          uint8_t *const dists, sig_t *dir_dignature_p,
                                          uint8_t *max_axis_p, uint8_t *max_distance_p) {

    //Cache variable, to avoid pointer access.
    uint8_t max_dist = 0;
    uint8_t max_axis = 0;
    sig_t dir_signature = 0;

    //We must determine the distance for every stepper
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        //get distance on stepper
        int32_t distance = pos[stepper] - dest[stepper];

        //get absolute distance
        if (distance < 0) {
            distance = -distance;
            dir_signature |= axis_signatures[stepper];
        }

        //if the distance is greater than the limit : fail with true
        if (distance >= MAXIMUM_DISTANCE_LIMIT) {
            return true;
        }

        //cast to small type
        uint8_t rdist = (uint8_t) distance;

        //persist distance
        dists[stepper] = rdist;

        //Update max dist
        if (rdist > max_dist) {
            max_dist = rdist;
            max_axis = stepper;
        }
    }

    //Finally update all data
    *max_axis_p = max_axis;
    *max_distance_p = max_dist;
    *dir_dignature_p = dir_signature;

    //No error
    return false;
}


//TODO CALL HIGH LEVEL FUNCTION, FOR SPEED GROUPS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


float RealTimeProcessor::get_hl_distance(float *hl_distances) {

    float square_dist_sum = 0;

    for (uint8_t stepper = 0; stepper < NB_AXIS; stepper++) {
        float dist = hl_distances[stepper];
        square_dist_sum += dist * dist;
    }

    return (float) sqrt(square_dist_sum);
}

/*
 * update_end_distances : this function updates distances to end point and jerk point.
 *
 *  It takes as arguments the absolute distances on each stepper of the next sub_movement, and their negative signature.
 *
 *      Reminder : the i_th bit of negative_signature is 1 if the distance on the i_th stepper is negative.
 *
 *  The global distance is given by the maximum of all distance on each stepper.
 *
 */

void RealTimeProcessor::update_end_distances(const sig_t negative_signatures, const uint8_t *elementary_dists) {

#define STEPPER(i, sig, ...)\
    if (negative_signatures&sig) {end_distances[i] += elementary_dists[i];}\
    else {end_distances[i] -= elementary_dists[i];}\


#include "../../config.h"

#undef STEPPER

}


//-----------------------------------------------Speed_Management-------------------------------------------------------
//TODO MODIFY THE SPEED ALGORITHM

/*
 * pre_process_speed : this function determines the correct time, for the movement passed as argument.
 *
 *  The movement is passed in he form of :
 *      - movement distance : the high level distance
 *      - stepper_distances : the effective stepper distances of the current movement;
 *
 *  It starts to determine the largest time window, at the current speeds. This window is the intersection of
 *      time windows for all steppers.
 *
 *  Finally, the regulation time is determined, according to the following rule :
 *      - if the machine must decelerate, the chosen time is the upper bound of the determined window.
 *      - if not, if the regulation time is outside of the regulation window, the new time is the corresponding bound;
 *      - if not, the current time is the regulation time.
 *
 */

float RealTimeProcessor::pre_process_speed(float movement_distance, const uint8_t *const stepper_distances) {


    //The regulation time, corresponding to the regulation speed;
    float regulation_time = movement_distance / regulation_speed;

    //The final time
    float new_time = 0;

    if (!jerk_point) {
        //The movement time at the current speed, and its inverse,
        float inv_time = current_speed / movement_distance;

        //Window computation variables
        float min_time = 0, max_time = 0;
        bool first_max = true;

        //Get the time bounds for each stepper;
        for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

            //Get the speed on last movement, on stepper [stepper]
            float act_speed = steppers_speeds[stepper];

            //Compute the speed corresponding to the current movement
            float next_speed = (float) stepper_distances[stepper] * inv_time;

            //Get the speed difference, its sign, and get its absolute value.
            float speed_diff = next_speed - act_speed;
            bool in_deceleration = (speed_diff > 0);
            if (in_deceleration) speed_diff = -speed_diff;

            //Get the speed limit
            float speed_limit = EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper];

            //Time window determination for all axis
            if (speed_diff > speed_limit) {

                if (in_deceleration) {

                    //if we decelerate, the new speed = act_speed - speed limit.
                    float n_time = stepper_distances[stepper] / (act_speed - speed_limit);

                    //update maximum time, as the minimum of the new time and the current max time :
                    min_time = (first_max) ? n_time : ((n_time < min_time) ? min_time : n_time);

                    first_max = false;

                } else {

                    //if we accelerate, the new speed = act_speed + speed limit.
                    float n_time = stepper_distances[stepper] / (act_speed + speed_limit);

                    //update minimum time, as the maximum of the new time and the current min time :
                    max_time = (n_time < max_time) ? n_time : max_time;
                }

            }

        }

        if (deceleration_required || (regulation_time > max_time)) {
            //If the deceleration is too high, or if the regulation time is higher than the maximum time :

            //Deceleration done
            deceleration_required = false;

            //choose the time corresponding to the maximum deceleration.
            new_time = max_time;

        } else if (regulation_time < min_time) {
            //If the regulation time is lower than the min time :

            //choose the time corresponding to the maximum acceleration.
            new_time = min_time;

        } else {
            //If the regulation time is in the time window :

            //choose the regulation time.
            new_time = regulation_time;
        }

    } else {

        //if the next point is a jerk point, no acceleration checking is required.
        new_time = regulation_time;
    }

    //Update the speed
    current_speed = movement_distance / new_time;

    //validate the acceleration management for the next movement.
    jerk_point = false;

    return new_time;

}


/*
 * plan_speed_change : this function marks that the next position to be pushed will be after a jerk point;
 *
 */

void RealTimeProcessor::plan_speed_change(float speed) {

    next_push_jerk = true;

    next_regulation_speed = speed;
}

/*
 * update_speeds : this function updates all stepper's speeds.
 *
 */

void RealTimeProcessor::update_speeds(const uint8_t *const stepper_distances, float time) {

    float inv_time = 1 / time;

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        //set the current speed on the stepper [stepper]

        //get the speed for the stepper
        float v = steppers_speeds[stepper] = (float) stepper_distances[stepper] * inv_time;

        //If a deceleration is required, no need for deceleration_distance checking
        if (deceleration_required)
            continue;

        //Compute the deceleration deceleration_distance on the stepper
        uint32_t deceleration_distance = (uint32_t) ((v * v) / (2 * EEPROMStorage::accelerations[stepper]));

        //require a deceleration if the end distance on the stepper is below the deceleration_distance;
        if (deceleration_distance > end_distances[stepper]) {
            deceleration_required = true;
        }
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

void RealTimeProcessor::pop_next_position(uint8_t *elementary_dists, sig_t *negative_signature, float *distance) {

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
        pre_processor_data *data = sub_movement_queue.peak();

        *negative_signature = data->negative_signature;
        *distance = data->distance;
        bool jp = data->jerk_point;
        jerk_point = jp;
        if (jerk_point) {
            regulation_speed = data->speed;
        }

        return;

    }

}

void RealTimeProcessor::reset_vars() {
    RealTimeProcessor::last_position_popped = RealTimeProcessor::last_position_processed = false;
}


//Static declarations - definitions;

#define m RealTimeProcessor

//Current stepper position;
int32_t t_cur_pos[NB_STEPPERS];
int32_t *const m::current_stepper_positions = t_cur_pos;

//movement data queue;
Queue<pre_processor_data> m::sub_movement_queue(MOTION_DATA_QUEUE_SIZE);

//the elementary distances data
uint8_t t_sm_d[MOTION_DATA_QUEUE_SIZE * NB_STEPPERS];
uint8_t *m::sub_movement_distances = t_sm_d;


//Indexation variables
float m::index_min = 0, m::index_max = 0, m::index = 0, m::increment = 0;

//Trajectory function
void (*m::get_new_position)(float, float *);

//Queue state
bool m::last_position_processed = false;
bool m::last_position_popped = false;


//End distances
int32_t k2ted[NB_STEPPERS]{0};
int32_t *const m::end_distances = k2ted;

bool m::deceleration_required = false;

bool m::jerk_point = true;

//Global speed
float m::regulation_speed;
float m::current_speed;

//next_speed parameters
bool m::next_push_jerk = false;
float m::next_regulation_speed = 0;

//Steppers speeds
float t_st_spd[NB_STEPPERS];
float *const m::steppers_speeds = t_st_spd;

//Deceleration distances
uint32_t t_dec_dst[NB_STEPPERS];
uint32_t *const m::deceleration_distances = t_dec_dst;


uint8_t m::linear_tools_nb;

void (*k2tf[NB_CONTINUOUS]);

void (**m::linear_set_functions)(float) = (void (**)(float)) k2tf;




//Axis signatures

#define STEPPER(i, sig, ...) sig,
sig_t k2t_sig[NB_STEPPERS + 1]{

#include "../../config.h"

        0};

#undef STEPPER

const sig_t *const m::axis_signatures = k2t_sig;

#undef m


#endif