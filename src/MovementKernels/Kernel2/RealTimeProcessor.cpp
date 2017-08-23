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

#ifdef ENABLE_STEPPER_CONTROL

#include "RealTimeProcessor.h"
#include "../../Actions/ContinuousActions.h"
#include "../../interface.h"
#include "../../Core/EEPROMStorage.h"
#include "MovementKernels/MachineAbstraction.h"
#include "ComplexTrajectoryExecuter.h"
#include "_kernel_2_data.h"


void RealTimeProcessor::start() {

    //initialise speeds and distances to zeros.
    memset(steppers_speeds, 0, NB_STEPPERS * sizeof(float));
    memset(deceleration_distances, 0, NB_STEPPERS * sizeof(uint32_t));

    //Initialise the last movement time
    last_time = 0.01;

    //Pre compute the speed constants
    pre_compute_speed_constants();


}


/*
 * send_position : this function sends the high level position to the controller.
 *
 *  It first inverts the current stepper positions, to obtain the high level position.
 *
 *  Then, it sends it using the interface.
 *
 */
void RealTimeProcessor::send_position() {

    //Send the current high level position
    CI::send_position(current_hl_position);
    //StepperController::send_position();


}


/*
 * set_regulation_speed : this function updates the regulation speed,
 */

void RealTimeProcessor::set_regulation_speed(uint8_t speed_group, float speed) {

    //Set the speed group
    RealTimeProcessor::movement_speed_group = speed_group;

    //Set the speed
    next_regulation_speed = speed;

}


/*
 * set_regulation_speed_jerk : this function updates the regulation speed,
 *      and marks that the next position to be pushed will be after a jerk point;
 *
 */

void RealTimeProcessor::set_regulation_speed_jerk(uint8_t speed_group, float speed) {

    //Program a jerk point
    next_jerk_flag = true;

    //Set the speed
    set_regulation_speed(speed_group, speed);
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
    index_limit = max;
    index = min;
    increment = incr;
    positive_index_dir = incr > 0;

    //update position provider
    get_new_position = trajectory_function;

    movement_processed = false;

    empty_queue = false;

}


//-----------------------------------Intermediary_Positions_Pre_Computation---------------------------------------------


void RealTimeProcessor::fill_sub_movement_queue() {

    while (sub_movement_queue.available_spaces() && !movement_processed) {
        push_new_position();
    }
}

/*
 * push_new_position_1 : this function gets the next position, defined by the image of index+increment
 *      by the trajectory function.
 *
 *      If the image is at a distance from the current position comprised on
 *          MINIMUM_DISTANCE_LIMIT and MAXIMUM_DISTANCE_LIMIT
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
 */

position_data_struct p = {};
position_data_struct *position_data = &p;


void RealTimeProcessor::push_new_position() {
    push_new_position_1();
    push_new_position_2();
}

void RealTimeProcessor::push_new_position_1() {

    if ((!sub_movement_queue.available_spaces()) || (movement_processed))
        return;

    //Stepper positions : the new position in the steppers system (translated by MachineAbstraction).

    //Stepper distances : As copying requires time, we directly put the temp distances in the final array.
    uint8_t push_indice = sub_movement_queue.push_indice();
    position_data->elementary_dists = sub_movement_int_distances + push_indice * NB_STEPPERS;
    position_data->real_dists = sub_movement_real_distances + push_indice * NB_STEPPERS;

    //The maximal stepper distance, and the maximum stepper


    //------------------Distances_computing-----------------------------


    //Get the new index candidate;
    float local_index_candidate = index + increment;

    if (((positive_index_dir) && (local_index_candidate + increment > index_limit)) ||
        ((!positive_index_dir) && (local_index_candidate + increment < index_limit))) {
        movement_processed = true;
        local_index_candidate = index_limit;
    }


    //Get the new high level position;
    get_new_position(local_index_candidate, position_data->candidate_high_level_positions);

    position_data->index_candidate = local_index_candidate;

    //Compute the movement distance for the current speed group
    position_data->movement_distance = get_movement_distance_for_group(movement_speed_group,
                                                           position_data->candidate_high_level_positions);


    //Translate the high level position into steppers position;
    MachineAbstraction::translate(position_data->candidate_high_level_positions, position_data->future_steppers_positions);


}

void RealTimeProcessor::push_new_position_2() {

    if ((!sub_movement_queue.available_spaces()) || (movement_processed))
        return;


    uint8_t max_axis;
    float max_distance;

    sig_t negative_signature;

    //Get the steppers distances, high level distances, and the maximal stepper and distance
    bool up_check = get_steppers_distances(current_stepper_positions, position_data->future_steppers_positions,
                                           position_data->elementary_dists, position_data->real_dists,
                                           &negative_signature, &max_axis, &max_distance);


    //-----------------Validity_Verification----------------------------

    if (max_distance != DISTANCE_TARGET) {

        //Increment adjustment according to the target
        increment = increment * (float) DISTANCE_TARGET / max_distance;

    }

    //If the maximal distance is below the lower limit :
    if (up_check) {
        movement_processed = false;
        return;
    }

    //If the maximal distance is below the lower limit :
    if ((!movement_processed) && (max_distance <= MINIMUM_DISTANCE_LIMIT)) {
        return;
    }

    //---------------------Movement_Enqueuing----------------------------

    /*
     * Now that validity checks are made, we can enqueue the sub_movement :
     * As the distances are already in the array, all we need to do is to push the direction signature
     *      and validate the candidate index.
     */

    k2_real_time_data *d = sub_movement_queue.get_push_ptr();

    d->negative_signature = negative_signature;
    d->distance = position_data->movement_distance;
    d->speed = next_regulation_speed;
    d->jerk_point = next_jerk_flag;

    //A jerk point is punctual (no shit !), so when he has been enqueued, set the flag to false;
    next_jerk_flag = false;

    sub_movement_queue.push();

    index = position_data->index_candidate;

    update_current_hl_position(position_data->candidate_high_level_positions);

}


/*
 * get_movement_distance_for_group : this function computes the movement distance for the movement provided in argument,
 *      in the cartesian group provided in argument.
 *
 *  The movement is provided in the form of its distances.
 *
 *  The distance in the group is defined as the norm2 of the distance vector's projected
 *      in the concerned cartesian group
 *
 * 10 us
 *
 */

float RealTimeProcessor::get_movement_distance_for_group(uint8_t speed_group, const float *const distances) {

    float square_dist_sum = 0;

    //Initialise the stepper index pointer
    const int8_t *indexes = (speed_groups_indices + 3 * speed_group);


    //Sum the square of all distance :
    for (uint8_t stepper = 0; stepper < 3; stepper++) {

        //Get the current axis value
        int8_t index = indexes[stepper];

        //if the cartesian group comprises less than 3 axis;
        if (index == -1) break;

        //get the distance
        float dist = distances[index] - current_hl_position[index];

        //update the square sum
        square_dist_sum += dist * dist;
    }

    //compute the square root and return it.
    float f = sqrtf(square_dist_sum);

    return f;
}


/*
 * get_steppers_distances : this function determines the distance between a position and a target, for all stepper.
 *
 * It also computes the direction signature, as distances are positive numbers, and saves the maximum stepper
 *      and the maximum distance.
 *
 */

bool
RealTimeProcessor::get_steppers_distances(float *const pos, const float *const dest, uint8_t *const int_dists,
                                          float *const real_dists, sig_t *dir_dignature_p, uint8_t *max_axis_p,
                                          float *max_distance_p) {

    //Cache variable, to avoid pointer access.
    uint8_t max_int_dist = 0;
    float max_f_dist = 0;
    uint8_t max_axis = 0;
    sig_t dir_signature = 0;

    //We must determine the distance for every stepper
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        //get destination and position
        float d = dest[stepper], p = pos[stepper];

        //update position with destination;
        pos[stepper] = d;

        //get the real distance
        float distance = d - p;

        uint8_t int_dist;

        //get absolute distance
        if (distance < 0) {
            distance = -distance;
            dir_signature |= axis_signatures[stepper];
            int_dist = (uint8_t) ((uint32_t) p - (uint32_t) d);
        } else {
            int_dist = (uint8_t) ((uint32_t) d - (uint32_t) p);
        }

        //if the distance is greater than the limit : fail with true
        if (distance >= MAXIMUM_DISTANCE_LIMIT) {
            return true;
        }

        //persist distances
        real_dists[stepper] = distance;
        int_dists[stepper] = int_dist;


        //Update max dist
        if (int_dist > max_int_dist) {
            max_int_dist = int_dist;
            max_f_dist = distance;
            max_axis = stepper;
        }
    }

    //Finally update all data
    *max_axis_p = max_axis;
    *max_distance_p = max_f_dist;
    *dir_dignature_p = dir_signature;

    //No error
    return false;
}


/*
 * update_current_hl_position : updates the current high level position with the provided one.
 */
void RealTimeProcessor::update_current_hl_position(float *new_hl_position) {

    memcpy(current_hl_position, new_hl_position, sizeof(float) * NB_AXIS);

}


/*
 * update_end_position : this function updates distances to end point and jerk point.
 *
 *  It takes in arguments the absolute distances on each stepper of the next sub_movement, and their negative signature.
 *
 *      Reminder : the i_th bit of negative_signature is 1 if the distance on the i_th stepper is negative.
 *
 *  The global distance is given by the maximum of all distance on each stepper.
 *
 */

void RealTimeProcessor::update_end_position(const float *const new_hl_position) {

    float stepper_end_position[NB_STEPPERS];

    MachineAbstraction::translate(new_hl_position, stepper_end_position);

    if (ComplexTrajectoryExecuter::started) {
        disable_stepper_interrupt();
    }

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        int32_t d = (int32_t) stepper_end_position[stepper];
        end_distances[stepper] += d - end_position[stepper];
        end_position[stepper] = d;
    }

    if (ComplexTrajectoryExecuter::started) {
        enable_stepper_interrupt()
    }
}


/*
 * update_end_distances : this function updates distances to end point and jerk point.
 *
 *  It takes in arguments the absolute distances on each stepper of the next sub_movement, and their negative signature.
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

/*
 * pre_process_speed : this function determines the correct time, for the movement passed in argument.
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

float RealTimeProcessor::pre_process_speed(float movement_distance, const float *const stepper_distances) {

    //The regulation time, corresponding to the regulation speed;
    float regulation_time = movement_distance / regulation_speed;

    //The final time
    float new_time = 0;

    if (!jerk_flag) {

        //Window computation variables
        float min_time = 0, max_time = 0;
        bool first = true;

        //Get the time bounds for each stepper;
        for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

            //Get the speed on last movement, on stepper [stepper]
            float act_speed = steppers_speeds[stepper];

            //Get the speed limit
            //Formula : delta_speed = acceleration * steps * time; acceleration * steps is pre-computed (optimisation)
            float max_delta_speed = delta_speed_constants[stepper] * last_time;

            //if the current stepper can stop, no processing is required :

            float s;

            if ((s = act_speed - max_delta_speed) > 0) {

                //update the maximum time, the new speed = act_speed - speed limit.
                //Formula : up_time_bound = stepper_distance / (actual_speed - max_delta_speed)
                float up_time = stepper_distances[stepper] / s;

                //update maximum time, as the minimum of the new time and the current max time :
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
            float down_time = stepper_distances[stepper] / maximum_speed;

            //update minimum time, as the maximum of the new time and the current min time :
            min_time = (down_time < min_time) ? min_time : down_time;


        }

        /*
         * A particular case we must care of, if when the bounds are inverted. It may result of :
         *  - a fast direction variation in the current trajectory, that makes an axis decelerate too much
         *      and another accelerating too much.
         *  - a jerk point speed update, where the target speed makes a stepper overflow its limit.
         *      As in jerk points, speed limits are not checked, it can happen.
         *
         * In this case, the deceleration is prioritised.
         *
         */

        //If the bounds are inverted, decelerate in priority.
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

            //If the regulation time is lower than the min time :

            //choose the time corresponding to the maximum acceleration.
            new_time = min_time;

        } else {


            //If the regulation time is in the time window :

            //choose the regulation time.
            new_time = regulation_time;
        }

    } else {

        //if the next point is a jerk point, no acceleration checking is required, only the maximum speed checking.
        new_time = regulation_time;


    }


    last_time = new_time;

    return new_time;

}


/*
 * update_speeds : this function updates all stepper's speeds.
 *
 */

void RealTimeProcessor::update_speeds(const float *const stepper_distances, float time) {

    //Only the inverse of time is used, computes now for optimisation purposes.
    float inv_time = 1 / time;


    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        //set the current speed on the stepper [stepper]

        //get the speed for the stepper
        //Formula : v = d / t -> v = d * (1 / t).
        float v = steppers_speeds[stepper] = stepper_distances[stepper] * inv_time;

        //If a deceleration is required, no need for deceleration_distance checking
        if (deceleration_required)
            continue;

        //Compute the deceleration deceleration_distance on the stepper
        //Formula : v * v / (2 * acceleration * steps); the denominator is pre-computed for optimisation purposes.
        uint32_t deceleration_distance = (uint32_t) (v * v * deceleration_constants[stepper]);

        //get the algebraic end distance.
        int32_t end_distance = end_distances[stepper];

        //get the absolute
        if (end_distance < 0) end_distance = -end_distance;

        //require a deceleration if the end distance on the stepper is below the deceleration_distance;
        if (deceleration_distance > (uint32_t) end_distance) {
            deceleration_required = true;
        }
    }
}


//---------------------------------------------------Speed_Constants----------------------------------------------------

void RealTimeProcessor::pre_compute_speed_constants() {

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        float steps = EEPROMStorage::steps[stepper];
        float as = EEPROMStorage::accelerations[stepper] * steps;
        max_speed_constants[stepper] = EEPROMStorage::maximum_speeds[stepper] * steps;
        delta_speed_constants[stepper] = as;
        deceleration_constants[stepper] = (float) 1 / ((float) 2 * as);
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
 * TIME OF EXECUTION : 2us with 4 steppers
 *
 * Costly operations : memcpys.
 */

void RealTimeProcessor::pop_next_position(uint8_t *elementary_dists, float *real_dists, sig_t *negative_signature,
                                          float *distance) {

    uint8_t size = sub_movement_queue.available_elements();

    if (!size) {

        //if the queue is empty : error
        CI::echo("ERROR : IN ComplexTrajectoryExecuter::pop_next_position : THE SUB_MOVEMENT QUEUE IS EMPTY");

        return;

    } else {

        //If the queue only contains the last sub_movement : disable the sub_movement pre_processing
        if ((size == 1) && (movement_processed)) {
            empty_queue = true;
        }

        //Get the current sub movement index of the queue, for arrays indexation
        uint8_t pull_index = sub_movement_queue.pull_indice();

        //The pointer to the beginning of pre-processed elementary distances
        uint8_t *e_ptr = sub_movement_int_distances + NB_STEPPERS * pull_index;
        float *r_ptr = sub_movement_real_distances + NB_STEPPERS * pull_index;

        //copy
        memcpy(elementary_dists, e_ptr, NB_STEPPERS * sizeof(uint8_t));
        memcpy(real_dists, r_ptr, NB_STEPPERS * sizeof(float));

        //update the direction signature and the distance memorised in the queue.
        k2_real_time_data *data = sub_movement_queue.peak();

        *negative_signature = data->negative_signature;
        *distance = data->distance;
        jerk_flag = data->jerk_point;
        regulation_speed = data->speed;

        sub_movement_queue.discard();

        return;

    }

}

//-----------------------------------------Static declarations - definitions--------------------------------------------

//Static declarations - definitions;

#define m RealTimeProcessor

//Current stepper position;
float t_cur_pos[NB_STEPPERS]{0};
float *const m::current_stepper_positions = t_cur_pos;

//Positions
float t_rl_pos[NB_AXIS]{0};
float *const m::current_hl_position = t_rl_pos;

//movement data queue;
Queue<k2_real_time_data> m::sub_movement_queue(MOTION_DATA_QUEUE_SIZE);

//the integer distances data
uint8_t t_sm_d[MOTION_DATA_QUEUE_SIZE * NB_STEPPERS]{0};
uint8_t *m::sub_movement_int_distances = t_sm_d;

//the real distances data
float t_sm_rd[MOTION_DATA_QUEUE_SIZE * NB_STEPPERS]{0};
float *m::sub_movement_real_distances = t_sm_rd;


//Indexation variables
float m::index_limit = 0, m::index = 0, m::increment = 0;
bool m::positive_index_dir = false;

//Speed group for the current movement
uint8_t m::movement_speed_group = 0;

//Trajectory function
void (*m::get_new_position)(float, float *);

//Queue state
bool m::movement_processed = false;
bool m::empty_queue = false;


//End distances
int32_t k2ted[NB_STEPPERS]{0};
int32_t *const m::end_distances = k2ted;

int32_t k2tep[NB_STEPPERS]{0};
int32_t *const m::end_position = k2tep;

bool m::deceleration_required = false;

bool m::jerk_flag = true;

//Global speed
float m::regulation_speed;
float m::last_time;

//next_speed parameters
bool m::next_jerk_flag = false;
float m::next_regulation_speed = 0;

//Steppers speeds
float t_st_spd[NB_STEPPERS];
float *const m::steppers_speeds = t_st_spd;

//Deceleration distances
uint32_t t_dec_dst[NB_STEPPERS];
uint32_t *const m::deceleration_distances = t_dec_dst;

//Axis signatures

#define STEPPER(i, sig, ...) sig,
sig_t k2t_sig[NB_STEPPERS + 1]{

#include "../../config.h"

        0};

#undef STEPPER

const sig_t *const m::axis_signatures = k2t_sig;


//Speed groups indices

//declare and fill the array
int8_t t_sg_indices[3 * NB_CARTESIAN_GROUPS + 1] = {

#define CARTESIAN_GROUP(i, a, b, c, s) a, b, c,

#include <config.h>

#undef CARTESIAN_GROUP
        //end the array
        0};

//Assign the array
const int8_t *const m::speed_groups_indices = t_sg_indices;


//Deceleration distances
float t_dec_const[NB_STEPPERS];
float *const m::deceleration_constants = t_dec_const;


//Deceleration distances
float t_dsp_const[NB_STEPPERS];
float *const m::delta_speed_constants = t_dsp_const;


//Deceleration distances
float t_msp_const[NB_STEPPERS];
float *const m::max_speed_constants = t_msp_const;


#undef m

#endif