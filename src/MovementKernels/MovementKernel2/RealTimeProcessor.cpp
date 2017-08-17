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

/*
 * Heuristic distance : this function calculates the heuristic distance between the actual position and the end position
 *
 * The current heuristic is the sum of all distances.
 */
void RealTimeProcessor::heuristic_end_distance() {

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

void RealTimeProcessor::heuristic_jerk_distance() {

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
    if (negative_signatures&sig) {end_distances[i] += elementary_dists[i];jerk_distances[i] += elementary_dists[i];}\
    else {end_distances[i] -= elementary_dists[i];jerk_distances[i] -= elementary_dists[i];}\


#include "../../config.h"

#undef STEPPER

    heuristic_end_distance();
    heuristic_jerk_distance();

}


//-----------------------------------------------Speed_Management-------------------------------------------------------
//TODO MODIFY THE SPEED ALGORITHM

float RealTimeProcessor::pre_process_speed(float movement_distance, const uint8_t *const stepper_distances) {


    if (deceleration_required) {
        //If the machine must decelerate, because of the proximity of a jerk/end point:

        //temp value for the deceleration stepper.
        uint8_t stepper = deceleration_axis;

        //the value sqrt(2*acceleration*stepspmm)
        float sqrt_2as = (float) sqrt(
                2 * EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper]);//TODO PRE_PROCESS

        //The square root of the deceleration distance
        float sqrt_dist = (float) sqrt(deceleration_dist); //TODO FASTSQRT

        //The delay time
        float time = (float) stepper_distances[stepper] / (sqrt_2as * sqrt_dist);

        //Update the speed
        current_speed = movement_distance / time;

        return time;

    } else if (acceleration_required) {
        //If the regulation speed hasn't been reached yet :

        float min_time = 0;

        /*
         * We will here determine the minimum time for the current movement.
         *
         *  To do this, we will calculate the min time on each stepper and determine the minimum of those.
         *
         *  After, we update the current speed
         *
         */

        for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

            //get the current speed on the stepper [stepper]
            float act_speed = steppers_speeds[stepper];

            //calculate the minimum time, according to the acceleration bound :
            float time = stepper_distances[stepper] /
                         (act_speed + EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper]);

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

        //Get the time bounds for each stepper;

        for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
            //get the current speed on the stepper [stepper]

            float act_speed = steppers_speeds[stepper];

            float next_speed = (float) stepper_distances[stepper] * inv_time;

            float speed_diff = next_speed - act_speed;

            bool in_deceleration = (speed_diff > 0);

            if (in_deceleration) speed_diff = -speed_diff;

            float speed_limit = EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper];

            //Acceleration or Deceleration limit reaching :
            if (speed_diff > speed_limit) {

                if (in_deceleration) {

                    //if we decelerate, the new speed = act_speed - speed limit.
                    float n_time = stepper_distances[stepper] / (act_speed - speed_limit);

                    //update maximum time, as the minimum of the new time and the current max time :
                    min_time = (first_max) ? n_time : ((n_time < min_time) ? min_time : n_time);

                    first_max = false;

                    max_reached = true;

                } else {

                    //if we accelerate, the new speed = act_speed + speed limit.
                    float n_time = stepper_distances[stepper] / (act_speed + speed_limit);

                    //update minimum time, as the maximum of the new time and the current min time :
                    max_time = (n_time < max_time) ? n_time : max_time;

                    min_reached = true;
                }

            }

        }

        float new_time = 0;

        if (max_reached) {
            //If the deceleration is too high, or if both limits are reached (impossible prepare_movement) : decelerate.

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


void RealTimeProcessor::update_speeds(const uint8_t *const stepper_distances, float time) {

    float inv_time = 1 / time;

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        //set the current speed on the stepper [stepper]

        float v = steppers_speeds[stepper] = (float) stepper_distances[stepper] * inv_time;

        deceleration_distances[stepper] = (uint32_t) ((v * v) / (2 * EEPROMStorage::accelerations[stepper]));//TODO PRE_PROCESS

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
        pre_processor_data data = sub_movement_queue.pull();

        *negative_signature = data.negative_signature;
        *distance = data.distance;

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


//jerk distances
int32_t k2tjd[NB_STEPPERS]{0};
int32_t *const m::jerk_distances = k2tjd;


//distances to end and jerk point
uint32_t m::distance_to_end_point = 0;
uint32_t m::offseted_distance_to_jerk_point = 0;


//Deceleration Fields
bool m::acceleration_required = false;
bool m::deceleration_required = false;//TODO SET THOSE VARS ?
float m::deceleration_dist = false;
uint8_t m::deceleration_axis = 0;


//Global speed
float m::current_speed;

//Steppers speeds
float t_st_spd[NB_STEPPERS];
float *const m::steppers_speeds = t_st_spd;

//Deceleration distances
uint32_t t_dec_dst[NB_STEPPERS];
uint32_t *const m::deceleration_distances = t_dec_dst;

//Speed Offset
uint32_t m::speed_offset;

//Jerk point watch
volatile bool m::watch_for_jerk_point;


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